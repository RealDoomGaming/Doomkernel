#include <task/task.h>
#include <stdlib.h>

// here we define how much each task gets for its private stack (4096 are 4 MiB)
#define STACK_SIZE_TASK 4096
// and here we define the max amount of tasks
#define MAX_TASKS 5

// list of all of our tasks
task_t task_list[MAX_TASKS];

void task_create(void (*entry)(void), uint16_t id) {
    // in this function we make a new task, but that should be self explenatory from the name
    task_t *task = &task_list[id];

    // setting the start and end of the tasks stack
    task->stack_start = malloc(STACK_SIZE_TASK);
    task->stack_top = (char *)task->stack_start + STACK_SIZE_TASK;

    // and then the other stuff
    task->id = id;
    task->state = TASK_READY;

    // havent decided if we should return or directly insert
}