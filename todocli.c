#include <stdio.h>
#include <string.h>
#include "todocli.h"

// Initialize the to-do list
void init_todo(TodoList *todo) {
    todo->count = 0;
}

// Add a task to the to-do list
void add_task(TodoList *todo, const char *task) {
    if (todo->count < MAX_TASKS) {
        strcpy(todo->tasks[todo->count], task);
        todo->count++;
        printf("Task added: %s\n", task);
    } else {
        printf("Error: Task list is full!\n");
    }
}

// Remove a task from the to-do list
void remove_task(TodoList *todo, const char *task) {
    int i, found = 0;
    for (i = 0; i < todo->count; i++) {
        if (strcmp(todo->tasks[i], task) == 0) {
            found = 1;
            break;
        }
    }
    if (found) {
        for (int j = i; j < todo->count - 1; j++) {
            strcpy(todo->tasks[j], todo->tasks[j + 1]);
        }
        todo->count--;
        printf("Task removed: %s\n", task);
    } else {
        printf("Task not found: %s\n", task);
    }
}

// List all tasks in the to-do list
void list_tasks(const TodoList *todo) {
    if (todo->count == 0) {
        printf("No tasks available.\n");
        return;
    }
    for (int i = 0; i < todo->count; i++) {
        printf("%d. %s\n", i + 1, todo->tasks[i]);
    }
}
