#pragma once

// cglm gl math

#include "../deps/cglm/cglm.h"

// see texture atlas file for textures available

typedef struct sprite_t {
    vec3 pos;
    vec2 sprite_coord;

    // ui sprites ignore camera position and draw exclusively in screen pixels

    bool ui;

    // resolution of sprite to pass into vert shader, support for wider textures and textures of different aspect ratios

    vec2 resolution;
} sprite;

// avoid tedious manual field filling, nice function. lookup table for sprites available

sprite make_sprite(vec3 pos, vec2 sprite_coord, bool ui, vec2 resolution);

// function where you pass in sprite data, will be added to drawcall queue in main

void draw_call(sprite s);

// call each frame

void draw_game();

// call at start of program

void init_rendering();

extern int render_game_width;