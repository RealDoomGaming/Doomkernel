#include <task/task.h>
#include <stdlib.h>
#include <string.h>
#include <timer/pit.h>

// here we define how much each task gets for its private stack (4096 are 4 MiB)
#define STACK_SIZE_TASK 4096
// and here we define the max amount of tasks
#define MAX_TASKS 5

// list of all of our tasks
task_t task_list[MAX_TASKS];
// which task is currently being "worked on"
uint16_t current_task = 0;
uint16_t task_count = 0;

void schedule(interrupt_frame_t *frame) {
    if (task_count == 0) {
        return;
    }

    // here we basically just freeze the current frame
    task_list[current_task].frame = *frame;
    // then we pick the next task to do
    current_task = (current_task + 1) % task_count;
    // but then we also need to load it
    *frame = task_list[current_task].frame;
}

void task_create(void (*entry)(void), uint16_t id) {
    if (id >= MAX_TASKS) {
        return;
    }

    // in this function we make a new task, but that should be self explenatory from the name
    task_t *task = &task_list[id];

    // setting the start and end of the tasks stack
    task->stack_start = malloc(STACK_SIZE_TASK);
    task->stack_top = (char *)task->stack_start + STACK_SIZE_TASK;
    // we should also zero out everything for that new frame
    memset(&task->frame, 0, sizeof(interrupt_frame_t));

    // rip is the instruction pointer which needs to hold a address and converting the function into uint64_t gives us the address
    task->frame.rip = (uint64_t)entry;
    // then the same for the stack pointer
    task->frame.rsp = (uint64_t)task->stack_top;
    // and then set all of the other flags
    task->frame.cs = kernel_frame_template.cs;
    task->frame.ss = kernel_frame_template.ss;
    task->frame.rflags = kernel_frame_template.rflags;

    // and then the other stuff
    task->id = id;
    task->state = TASK_READY;
    task_count++;
}