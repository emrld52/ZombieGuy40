#pragma once

// libraries

#include <stdbool.h>

// source files

#include "r_renderfuncs.h"

// dimensions all tilemaps must be, make height slightly taller than game window just so when screen shakes you dont see ugly edge

#define LEVELS_WIDTH 20
#define LEVELS_HEIGHT 16

// resolution of tiles on either axis. tiles must be square in dimensions

#define LEVELS_TILE_RESOLUTION 32

typedef struct tile_t {
    bool is_filled;
    int direction_for_pathing;

    // automatically figured out by autotiler

    sprite tile_sprite;
} tile;

void init_tilemap(tile arr[LEVELS_HEIGHT][LEVELS_WIDTH]);

// function called after loading level that visually stitches tiles together

void autotiler_build_tilemap(tile arr[LEVELS_HEIGHT][LEVELS_WIDTH]);

void render_tilemap(tile arr[LEVELS_HEIGHT][LEVELS_WIDTH]);