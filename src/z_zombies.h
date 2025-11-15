#pragma once

// libs

#include <stdbool.h>
#include "../deps/cglm/cglm.h"

#include "r_renderfuncs.h"
#include "s_entities.h"

#define TIME_KINGS_SPAWN 240
#define TIME_RANGERS_SPAWN 120
#define TIME_RANGERS_SPAWN 60

#define MAX_ZOMBIES 64

typedef struct zombie_t {
    entity *zmb;
    int tier;

    // only perform some logic only once every second to save on performance and also to stop zombies jumping right the frame after the player does

    float logic_frame_time;

    float speed;
    float time_til_next_jump_impulse;
    float jump_height;
    bool enabled;
} zombie;

extern zombie zombie_pool[MAX_ZOMBIES];

void spawn_zombie(int tier, int hit_points, float speed, float jump_height);
void simulate_zombies(entity *player);
void render_zombies();