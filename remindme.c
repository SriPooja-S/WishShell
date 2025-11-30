#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *reminder_thread(void *arg) {
    char *reminder = (char *)arg;
    char message[512]; // Increased buffer size to avoid truncation warning
    int time_delay = 0;
    char reminder_text[256];

    // Extract the last word as time and the rest as the reminder message
    char *last_space = strrchr(reminder, ' ');
    if (last_space) {
        *last_space = '\0'; // Split string into reminder text and time
        strcpy(reminder_text, reminder);
        time_delay = atoi(last_space + 1); // Convert time to integer
    } else {
        printf("Invalid format. Usage: remindme <message> <time in seconds>\n");
        free(reminder);
        return NULL;
    }

    snprintf(message, sizeof(message), "Reminder set: %s in %d seconds\n", reminder_text, time_delay);
    printf("%s", message);
    fflush(stdout);

    sleep(time_delay);

    // Print the reminder and then the wish> prompt
    printf("Reminder: %s\n", reminder_text);
    printf("wish> "); // Add this line to print the prompt after the reminder
    fflush(stdout);

    free(reminder);
    return NULL;
}

void remindme(char *input) {
    pthread_t thread;
    char *reminder_copy = strdup(input); // Duplicate input for safe processing

    if (pthread_create(&thread, NULL, reminder_thread, reminder_copy) != 0) {
        perror("Failed to create reminder thread");
        free(reminder_copy);
    }

    pthread_detach(thread); // Auto cleanup thread after execution
}
