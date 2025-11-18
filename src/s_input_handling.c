// src

#include "s_input_handling.h"
#include "g_state.h"

// libs

#include "../deps/sokol_app.h"

// pass input into a globally accessible input state, used as event callback by sokol

void program_event(const sapp_event* ev) {
    switch (ev->type) {
        case SAPP_EVENTTYPE_KEY_DOWN:
            if (ev->key_code < 512) {
                global_input.keys_pressed[ev->key_code] = true;
                global_input.key_tracker[ev->key_code] = false;
            }
            break;

        case SAPP_EVENTTYPE_KEY_UP:
            if (ev->key_code < 512) {
                global_input.keys_pressed[ev->key_code] = false;
                if(!global_input.key_tracker[ev->key_code]) {
                    global_input.keys_released[ev->key_code] = true;
                    global_input.key_tracker[ev->key_code] = true;
                }
            }
            break;
        
        case SAPP_EVENTTYPE_MOUSE_DOWN:
            if (ev->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
                global_input.mouse_l = true;
                global_input.mouse_l_tracker = false;
            }
            else if (ev->mouse_button == SAPP_MOUSEBUTTON_RIGHT) {
                global_input.mouse_r = true;
                global_input.mouse_r_tracker = false;
            }
            break;

        case SAPP_EVENTTYPE_MOUSE_UP:
            if (ev->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
                global_input.mouse_l = false;
                global_input.mouse_r = false;
                if(!global_input.mouse_l_up) {
                    global_input.mouse_l_up = true;
                    global_input.mouse_l_tracker = true;
                }
            }
            else if (ev->mouse_button == SAPP_MOUSEBUTTON_RIGHT) {
                global_input.mouse_r = false;
                if(!global_input.mouse_r_up) {
                    global_input.mouse_r_up = true;
                    global_input.mouse_r_tracker = true;
                }
            }
            break;

        case SAPP_EVENTTYPE_MOUSE_ENTER:
            sapp_show_mouse(false);
            break;

        case SAPP_EVENTTYPE_MOUSE_LEAVE:
            sapp_show_mouse(true);
            break;

        default:
            
            break;
    }

    global_input.mouse_x = (ev->mouse_x / sapp_width()) * VIRTUAL_WIDTH;
    global_input.mouse_y = (ev->mouse_y / sapp_height()) * VIRTUAL_HEIGHT;
}

// remove released input after 1 frame

void free_released_keys()
{
    for(int i = 0; i < 512; i++)
    {
        global_input.keys_released[i] = false;
    }

    global_input.mouse_l_up = false;
    global_input.mouse_r_up = false;
}