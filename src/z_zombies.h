#pragma once

// libs

#include <stdbool.h>
#include "../deps/cglm/cglm.h"

#include "r_renderfuncs.h"
#include "s_entities.h"

#define TIME_KINGS_SPAWN 240
#define TIME_RANGERS_SPAWN 120
#define TIME_RANGERS_SPAWN 60

#define MAX_ZOMBIES 256

// THE THREE ZOMBIE TYPES ARE MINION, RANGER AND KING

#define MINION_HP_GAIN_TIME 90
#define RANGER_HP_GAIN_TIME 70
#define KING_HP_GAIN_TIME 60

// only spawn rangers and kings after set amount of time

#define TIME_TIL_RANGER 60
#define TIME_TIL_KING 180

#define STARTING_ZOMBIE_SPAWN_TIME 3
#define SPAWN_TIME_MULT_RATE 1.016f

typedef struct zombie_t {
    entity *zmb;
    int tier;

    // only perform some logic only once every second to save on performance and also to stop zombies jumping right the frame after the player does

    float logic_frame_time;

    float speed;
    float time_til_next_jump_impulse;
    float jump_height;

    vec2 last_used_tile_index;

    bool enabled;
} zombie;

extern zombie zombie_pool[MAX_ZOMBIES];

void spawn_zombie(int tier, int hit_points, float speed, float jump_height);
void simulate_zombies(entity *player);
void render_zombies();