#pragma once

#include <stdbool.h>
#include <stdint.h>

// cglm gl math

#include "../deps/cglm/cglm.h"

// big array which contains all keys currently pressed

typedef struct input_reader_t {
    bool keysPressed[512];
} input_reader;

extern input_reader global_input;

// delta time in ticks
extern uint64_t global_raw_delta_time;

// delt-time out of 1 second as a float, can multiply values by to get them over 1 second (e.g move 10 units a sec = 10 * delta_time)
extern float global_delta_time;

// game-speed in the event that i want to slow down or speed up time, maybe some kind of power up or death animation, im unsure
extern float global_game_speed;

// camera pos for screen shake and such
extern vec2 global_camera_position;