#ifndef _TASK_H
#define _TASK_H

#include <interrupts/isr.h>
#include <stdint.h>

// the enum for the different states a task can have, for now we only really need the first two
typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED
} task_state_t;

// the struct for the tasks
typedef struct {
    interrupt_frame_t frame;
    void *stack_start;
    void *stack_top;
    task_state_t state;
    int16_t id;
} task_t;

// since I am not sure how the compiler handels pointers to function in the h file I will write everything including the parameter names 
// which I dont usually do
void task_create(void (*entry)(void));
// for scheduling the tasks
void schedule(interrupt_frame_t*);

#endif