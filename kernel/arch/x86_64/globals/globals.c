#include <globals.h>

output_state DISPLAY_STATE;

void set_VBE() {
    DISPLAY_STATE = output_state.VBE;
}

void set_VGA() {
    DISPLAY_STATE = output_state.VGA;
}