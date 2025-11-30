//dupfinder.c
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "dupfinder.h"

#define MAX_FILES 1000

typedef struct {
    char path[512];
    unsigned char checksum;
} FileEntry;

FileEntry files[MAX_FILES];
int file_count = 0;
int duplicate_found = 0;

// Function to compute a simple checksum
unsigned char compute_checksum(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return 0;

    unsigned char checksum = 0, byte;
    while (fread(&byte, 1, 1, file)) {
        checksum ^= byte;
    }
    fclose(file);
    return checksum;
}

// Function to scan a directory
void scan_directory(const char *dir_path) {
    struct dirent *entry;
    DIR *dir = opendir(dir_path);
    if (!dir) return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat path_stat;
        stat(full_path, &path_stat);

        if (S_ISREG(path_stat.st_mode)) {
            unsigned char checksum = compute_checksum(full_path);

            for (int i = 0; i < file_count; i++) {
                if (files[i].checksum == checksum) {
                    printf("Duplicate found:\n  %s\n  %s\n\n", files[i].path, full_path);
                    duplicate_found = 1;
                }
            }

            strcpy(files[file_count].path, full_path);
            files[file_count].checksum = checksum;
            file_count++;
        } else if (S_ISDIR(path_stat.st_mode)) {
            scan_directory(full_path);
        }
    }
    closedir(dir);
}

// Function to be called from wish.c
void dupfinder(char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        printf("Invalid directory path provided: %s\n", dir_path);
        return;
    }
    closedir(dir);

    file_count = 0;
    duplicate_found = 0;

    scan_directory(dir_path);

    if (!duplicate_found) {
        printf("No duplicate files found in: %s\n", dir_path);
    }
}
