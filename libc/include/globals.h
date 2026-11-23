#ifndef __GLOBALS_H
#define __GLOBALS_H

typedef enum {
    STATE_VBE, STATE_VGA
} output_state;

extern output_state DISPLAY_STATE;

void set_VBE();
void set_VGA();


#endif