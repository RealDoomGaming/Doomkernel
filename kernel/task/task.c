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

int16_t get_next_task() {
    uint16_t task = current_task;

    for (int16_t i = 0; i < task_count; i++) {
        task = (int16_t)((task + 1) % task_count);
        if (task_list[task].state != TASK_DONE) {
            return task
        }
    }

    return current_task;
}

void schedule(interrupt_frame_t *frame) {
    if (task_count == 0) {
        return;
    }

    // if the scheduling is on then we can schedule stuff like normal
    if (scheduler_started) {
        // here we basically just freeze the current frame
        task_list[current_task].frame = *frame;
        // then we pick the next task to do
        current_task = get_next_task();
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

void task_exit() {
    // when we exit a task we have to mark it as done
    task_list[current_task].state = TASK_DONE;
    
    // and halt the process or something
    while (1) {
        __asm__ volatile("hlt");
    }
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

void shift_task_list_down(uint16_t start) {
    uint16_t current = start;

    for (int16_t i = start; i < task_count - 1; i++) {
        task_list[current] = task_list[current + 1];
        task_list[current].id = current;
        current++;
    }
}

void reap(uint16_t task) {
    // in this function we basically free the memory and remove the task

    // firstly we free the memory in the tasks stack
    free(task_list[task].stack_start);

    // since this code is really really sensitive we have to disable interrupts here
    __asm__ volatile ("cli");

    // then we need to shift everything past this current task down one
    shift_task_list_down(task);

    // then we shrink task count
    task_count--;
    // and we also shrink the current task down by one
    // but before we can do that we have to firstly check if the current task is bigger then the task we want to remove
    // or if current task is at the last task we have
    if (current_task > task) {
        current_task--;
    } else if (current_task >= task_count) {
        current_task = 0;
    }

    // we enabled interrupts again once we are done
    __asm__ volatile ("sti");
}

void task_reap() {

    for (int16_t i = 0; i < task_count; i++) {
        if (task_list[i].state == TASK_DONE) {
            reap(i);
            i--;
        }
    }
}

void task_reaper() {
    // in this function we check periodically if any tasks are done with the task_reap function
    while (1) {
        task_reap();
        for (int i = 0; i < 1000000; i++);
    }
}