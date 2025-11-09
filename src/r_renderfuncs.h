// cglm gl math

#include "../deps/cglm/cglm.h"

typedef struct sprite_t {
    vec3 pos;
} sprite;

// function where you pass in sprite data, will be added to drawcall queue in main

void draw_call(sprite s);

// call each frame

void draw_game();

// call at start of program

void init_rendering();