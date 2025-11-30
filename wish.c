#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "dupfinder.h"
#include "netstatus.h"
#include "wcleanup.h"
#include "todocli.h" // Include the to-do list header
#include "remindme.h"

#define MAX_INPUT 256
#define TEST_HOST "google.com"
#define ALT_HOST "cloudflare.com"
#define MAX_ARGS 100
#define MAX_PATHS 10

char *path[MAX_PATHS] = {"/bin", "/usr/bin", NULL};
TodoList todo; // Global to-do list

void execute_command(char **args);
void change_directory(char **args);
void run_shell(FILE *input);
void handle_piped_commands(char *line);
void cleanup();

int main(int argc, char *argv[]) {
    FILE *input = (argc == 1) ? stdin : fopen(argv[1], "r");
    if (!input) {
        write(STDERR_FILENO, "An error has occurred\n", 22);
        exit(1);
    }
    init_todo(&todo);
    run_shell(input);
    if (input != stdin) fclose(input);
    cleanup(); // Free memory allocated for path if dynamically allocated
    return 0;
}

void run_shell(FILE *input) {
    char *line = NULL;
    size_t len = 0;

    while (1) {
        if (input == stdin) printf("wish> ");
        if (getline(&line, &len, input) == -1) {
            free(line);
            exit(0);
        }

        char *args[MAX_ARGS], *token = strtok(line, " \t\n");
        int i = 0;
        while (token) {
            args[i++] = token;
            token = strtok(NULL, " \t\n");
        }
        args[i] = NULL;
        if (i == 0) continue;

        if (strcmp(args[0], "exit") == 0) {
            free(line);
            exit(0);
        }
        if (strcmp(args[0], "cd") == 0) {
            change_directory(args);
            continue;
        }

        execute_command(args);
    }
}

void execute_command(char **args) {
    if (strcmp(args[0], "dupfinder") == 0) {
        if (args[1] == NULL) {
            printf("Usage: dupfinder <directory-path>\n");
        } else {
            dupfinder(args[1]);
        }
        return;
    }

    if (strcmp(args[0], "netstatus") == 0) {
        printf("Checking internet connection...\n");
        if (check_connection(TEST_HOST) || check_connection(ALT_HOST)) {
            printf("Internet: Connected\n");
            get_public_ip();
            get_interface_info();
            check_latency();
        } else {
            printf("Internet: Disconnected\n");
        }
        return;
    }

    if (strcmp(args[0], "wcleanup") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "Usage: wcleanup <filename>\n");
            return;
        }
        if (args[2] != NULL && strcmp(args[2], "--tabs") == 0) {
            if (args[3] != NULL) {
                int tab_spaces = atoi(args[3]);
                if (tab_spaces <= 0) {
                    tab_spaces = 4;
                }
                wcleanup(args[1], tab_spaces);
            } else {
                fprintf(stderr, "Invalid tab size.\n");
            }
        } else if (args[2] != NULL) {
            fprintf(stderr, "Error: Only one filename allowed.\n");
        } else {
            wcleanup(args[1], -1);
        }
        return;
    }

    if (strcmp(args[0], "todocli") == 0) {
        if (args[1] == NULL ||
            (strcmp(args[1], "list") != 0 &&
             strcmp(args[1], "add") != 0 &&
             strcmp(args[1], "remove") != 0)) {
            printf("Usage: todocli [list|add|remove] [task]\n");
            return;
        }
        if (strcmp(args[1], "list") == 0) {
            list_tasks(&todo);
        } else if (strcmp(args[1], "add") == 0) {
            if (args[2] != NULL) {
                add_task(&todo, args[2]);
            } else {
                printf("Usage: todocli add [task]\n");
            }
        } else if (strcmp(args[1], "remove") == 0) {
            if (args[2] != NULL) {
                remove_task(&todo, args[2]);
            } else {
                printf("Usage: todocli remove [task]\n");
            }
        }
        return;
    }
    if (strcmp(args[0], "remindme") == 0) {
        if (args[1] == NULL || args[2] == NULL) {
            printf("Usage: remindme [task] [seconds]\n");
            return;
        }
        int seconds = atoi(args[2]);
        if (seconds <= 0) {
            printf("Error: Please enter a valid time in seconds.\n");
            return;
        }
        char reminder_command[MAX_INPUT];
        snprintf(reminder_command, sizeof(reminder_command), "%s %d", args[1], seconds);
        remindme(reminder_command);
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        for (int i = 0; path[i] != NULL; i++) {
            char full_path[100];
            snprintf(full_path, sizeof(full_path), "%s/%s", path[i], args[0]);
            if (access(full_path, X_OK) == 0) {
                execv(full_path, args);
            }
        }
        write(STDERR_FILENO, "An error has occurred\n", 22);
        exit(1);
    } else {
        wait(NULL);
    }
}

void change_directory(char **args) {
    if (args[1] == NULL) {
        write(STDERR_FILENO, "An error has occurred\n", 22);
        return;
    }
    if (chdir(args[1]) != 0) {
        write(STDERR_FILENO, "An error has occurred\n", 22);
    }
}

void cleanup() {
    for (int i = 0; i < MAX_PATHS; i++) {
        if (path[i]) {
            free(path[i]);
            path[i] = NULL;
        }
    }
}

void handle_piped_commands(char *line) {
    char *cmd1 = strtok(line, "|");
    char *cmd2 = strtok(NULL, "|");

    if (!cmd1 || !cmd2) {
        write(STDERR_FILENO, "An error has occurred\n", 22);
        return;
    }

    int fd[2];
    if (pipe(fd) == -1) {
        write(STDERR_FILENO, "An error has occurred\n", 22);
        return;
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        char *args[MAX_ARGS];
        int i = 0;
        char *token = strtok(cmd1, " \t\n");
        while (token) {
            args[i++] = token;
            token = strtok(NULL, " \t\n");
        }
        args[i] = NULL;

        execute_command(args);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);

        char *args[MAX_ARGS];
        int i = 0;
        char *token = strtok(cmd2, " \t\n");
        while (token) {
            args[i++] = token;
            token = strtok(NULL, " \t\n");
        }
        args[i] = NULL;

        execute_command(args);
        exit(0);
    }

    close(fd[0]);
    close(fd[1]);
    wait(NULL);
    wait(NULL);
}
