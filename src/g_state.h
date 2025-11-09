#pragma once

#include <stdbool.h>
#include <stdint.h>

// big array which contains all keys currently pressed

typedef struct input_reader_t {
    bool keysPressed[512];
} input_reader;

extern input_reader global_input;

// delta time in ticks
extern uint64_t raw_delta_time;

// delt-time out of 1 second as a float, can multiply values by to get them over 1 second (e.g move 10 units a sec = 10 * delta_time)
extern uint64_t delta_time;