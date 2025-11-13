#pragma once

// cglm gl math

#include "../deps/cglm/cglm.h"

#define VIRTUAL_WIDTH 640
#define VIRTUAL_HEIGHT 480

#define MAX_DRAW_CALLS 512

// how many sprites horizontally and vertically (rows and columns)

#define TEXTURE_ATLAS_SPRITE_X_COUNT 20
#define TEXTURE_ATLAS_SPRITE_Y_COUNT 20

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

sprite make_sprite(vec2 pos, vec2 sprite_coord, bool ui, vec2 resolution);

// function where you pass in sprite data, will be added to drawcall queue in main

void draw_call(sprite s);

// call each frame

void draw_game();

// call at start of program

void init_rendering();

// camera shake function, jolts camera based on a magnitude

void camera_shake(float magnitude);

extern int render_game_width;