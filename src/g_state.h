#pragma once

// libs

#include <stdbool.h>
#include <stdint.h>
#include "../deps/cglm/cglm.h"

// source

#include "s_scene.h"

// big array which contains all keys currently pressed

typedef struct input_reader_t {
    bool keys_pressed[512];
    bool key_tracker[512];
    bool keys_released[512];
    float mouse_x;
    float mouse_y;
    bool mouse_l;
    bool mouse_r;
    bool mouse_l_up;
    bool mouse_r_up;
    bool mouse_l_tracker;
    bool mouse_r_tracker;
} input_reader;

extern input_reader global_input;

// delta time in ticks
extern uint64_t global_raw_delta_time;

// delt-time out of 1 second as a float, can multiply values by to get them over 1 second (e.g move 10 units a sec = 10 * delta_time)
extern float global_delta_time;

// currently loaded scene
extern scene *loaded_scene;

// pause functionality by setting scene speed to 0
extern bool is_paused;