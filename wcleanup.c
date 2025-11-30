#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "wcleanup.h"

#define DEFAULT_TAB_SPACES 4
#define MAX_LINE 1024

void remove_extra_whitespace(FILE *input, FILE *output, int tab_spaces);
void convert_tabs(char *line, int tab_spaces);

void wcleanup(const char *filename, int tab_spaces) {
    FILE *input = fopen(filename, "r");
    if (!input) {
        perror("Error opening file");
        return;
    }

    FILE *temp = tmpfile();
    if (!temp) {
        perror("Error creating temporary file");
        fclose(input);
        return;
    }

    remove_extra_whitespace(input, temp, tab_spaces);

    fclose(input);
    input = fopen(filename, "w");  // Reopen for writing
    if (!input) {
        perror("Error reopening file");
        fclose(temp);
        return;
    }

    rewind(temp);
    char ch;
    while ((ch = fgetc(temp)) != EOF) {
        fputc(ch, input);
    }

    fclose(input);
    fclose(temp);
}

void remove_extra_whitespace(FILE *input, FILE *output, int tab_spaces) {
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), input)) {
        char *start = line;
        char *end = line + strlen(line) - 1;

        while (end > start && isspace(*end)) {
            *end-- = '\0';
        }

        while (*start && isspace(*start)) {
            start++;
        }

        char cleaned[MAX_LINE] = {0};
        int j = 0, in_space = 0;

        for (int i = 0; start[i] != '\0'; i++) {
            if (isspace(start[i])) {
                if (!in_space) {
                    cleaned[j++] = ' ';
                    in_space = 1;
                }
            } else {
                cleaned[j++] = start[i];
                in_space = 0;
            }
        }
        cleaned[j] = '\0';

        if (tab_spaces > 0) {
            convert_tabs(cleaned, tab_spaces);
        }

        fprintf(output, "%s\n", cleaned);
    }
}

void convert_tabs(char *line, int tab_spaces) {
    char temp[MAX_LINE] = {0};
    int j = 0;

    for (int i = 0; line[i] != '\0'; i++) {
        if (line[i] == '\t') {
            for (int k = 0; k < tab_spaces; k++) {
                temp[j++] = ' ';
            }
        } else {
            temp[j++] = line[i];
        }
    }
    temp[j] = '\0';
    strcpy(line, temp);
}

