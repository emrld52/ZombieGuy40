// src

#include "s_input_handling.h"
#include "g_state.h"
#include "r_renderfuncs.h"

// libs

#include "../deps/sokol_app.h"

// pass input into a globally accessible input state, used as event callback by sokol

void program_event(const sapp_event* ev) {
    switch (ev->type) {
        case SAPP_EVENTTYPE_KEY_DOWN:
            if (ev->key_code < SAPP_MAX_KEYCODES) {
                global_input.keys_pressed[ev->key_code] = true;
                global_input.keys_released[ev->key_code] = false;

                if(!global_input.key_tracker[ev->key_code]) {
                    global_input.keys_clicked[ev->key_code] = true;
                    global_input.key_tracker[ev->key_code] = true;
                }
            }
            break;

        case SAPP_EVENTTYPE_KEY_UP:
            if (ev->key_code < SAPP_MAX_KEYCODES) {
                global_input.keys_pressed[ev->key_code] = false;
                global_input.key_tracker[ev->key_code] = false;
                global_input.keys_released[ev->key_code] = true;
            }
            break;
        
        case SAPP_EVENTTYPE_MOUSE_DOWN:
            if (ev->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
                global_input.mouse_l = true;
                global_input.mouse_l_tracker = false;
                if(!global_input.mouse_l_tracker) {
                    global_input.mouse_l_up = true;
                    global_input.mouse_l_tracker = true;
                }
            }
            break;

        case SAPP_EVENTTYPE_MOUSE_UP:
            if (ev->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
                global_input.mouse_l = false;
                global_input.mouse_l_tracker = false;
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

    // get mouse pos with scaled window accounted for

    float viewport_x, viewport_y;

    viewport_x = (screen_width - (screen_height * target_aspect)) * 0.5f;
    viewport_y = 0.0f;

    global_input.mouse_x = (ev->mouse_x - viewport_x) * ((float)VIRTUAL_WIDTH / (screen_height * target_aspect));
    global_input.mouse_y = (ev->mouse_y - viewport_y) * ((float)VIRTUAL_HEIGHT / sapp_height());
}

// remove released input after 1 frame

void free_released_keys()
{
    for(int i = 0; i < 512; i++)
    {
        global_input.keys_clicked[i] = false;
    }

    global_input.mouse_l_up = false;
    global_input.mouse_r_up = false;
}