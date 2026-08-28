#include <task/task.h>
#include <stdlib.h>
#include <string.h>
#include <timer/pit.h>
#include <stdio.h>

// here we define how much each task gets for its private stack (4096 are 4 MiB)
#define STACK_SIZE_TASK 4096
// and here we define the max amount of tasks
#define MAX_TASKS 10

// list of all of our tasks
task_t task_list[MAX_TASKS];
// which task is currently being "worked on"
int16_t current_task = 0;
int16_t task_count = 0;
// this is so we know when the scheduling was started
static uint8_t scheduler_started = 0;

void schedule(interrupt_frame_t *frame) {
    if (task_count == 0) {
        return;
    }

    // if the scheduling is on then we can schedule stuff like normal
    if (scheduler_started) {
        // here we basically just freeze the current frame
        task_list[current_task].frame = *frame;
        // then we pick the next task to do
        current_task = (current_task + 1) % task_count;
    } else {
        // and else we turn it on
        scheduler_started = 1;
    }

    // but then we also need to load it
    *frame = task_list[current_task].frame;
}

int16_t task_get_next_id() {
    if (task_count >= MAX_TASKS) {
        return -1;
    }

    return task_count;
}

void task_create(void (*entry)(void)) {
    int16_t id = task_get_next_id();

    if (id < 0) {
        printf("[task] no free task slots\n");
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