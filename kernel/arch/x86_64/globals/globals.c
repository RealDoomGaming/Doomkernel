#include <globals.h>

output_state DISPLAY_STATE = STATE_VGA;

void set_VBE() {
    DISPLAY_STATE = STATE_VBE;
}

void set_VGA() {
    DISPLAY_STATE = STATE_VGA;
}