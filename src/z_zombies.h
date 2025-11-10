#pragma once

// libs

#include <stdbool.h>
#include "../deps/cglm/cglm.h"

#include "r_renderfuncs.h"

#define MAX_ZOMBIES 48

typedef struct zombie_t {
    sprite zombie_sprite;
    vec2 vel;
    int tier;
    int hit_points;

    // only perform some logic only once every second to save on performance and also to stop zombies jumping right the frame after the player does

    float logic_frame_time;

    float speed;
    float time_til_next_jump_impulse;
    float jump_height;
    bool enabled;
    bool is_grounded;
} zombie;

extern zombie zombie_pool[MAX_ZOMBIES];

void spawn_zombie(int tier, int hit_points, float speed, float jump_height);
void simulate_zombies(sprite player);
void render_zombies();