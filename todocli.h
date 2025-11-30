#ifndef TODOCLI_H
#define TODOCLI_H

#define MAX_TASKS 100
#define TASK_LENGTH 256

// Struct for the to-do list
typedef struct {
    char tasks[MAX_TASKS][TASK_LENGTH];
    int count;
} TodoList;

void init_todo(TodoList *todo);
void add_task(TodoList *todo, const char *task);
void remove_task(TodoList *todo, const char *task);
void list_tasks(const TodoList *todo);

#endif // TODOCLI_H
