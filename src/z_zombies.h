#pragma once

// libs

#include <stdbool.h>
#include "../deps/cglm/cglm.h"

#include "r_renderfuncs.h"
#include "s_entities.h"

#define STARTING_MAX_ZOMBIES 10
#define MAX_ZOMBIES 48
#define ADD_ZOMBIE_LIMIT_PER_MINUTE 1
#define ZOMBIE_LOGIC_TICK_RATE 5

// THE THREE ZOMBIE TYPES ARE MINION, RANGER AND KING

#define MINION_HP_GAIN_TIME 90
#define RANGER_HP_GAIN_TIME 70
#define KING_HP_GAIN_TIME 60

#define MINION_BASE_HP 2
#define RANGER_BASE_HP 1
#define KING_BASE_HP 2

// at the moment just use deafult zombie values with no variation, can be easily expanded upon though

#define DEAFULT_ZOMBIE_SPEED 72.0f 
#define DEAFULT_ZOMBIE_JUMP_HEIGHT 512.0f * 0.75f

// only spawn rangers and kings after set amount of time

#define TIME_TIL_RANGER 60
#define TIME_TIL_KING 120

#define STARTING_ZOMBIE_SPAWN_TIME 3
#define SPAWN_TIME_MULT_RATE 1.01f

#define RANGER_ATTACK_SPEED 1.2f;

typedef enum zombie_position_relative_to_player
{
    ON_LEVEL_WITH_PLAYER = 0,
    ABOVE_PLAYER = 1,
    BELOW_PLAYER = 2
} zom_pos;

typedef enum zom_tier
{
    MINION_TIER = 1,
    RANGER_TIER = 2,
    KING_TIER = 3
} zombie_tier;

typedef struct zombie_t {
    entity *zmb;
    int tier;

    // only perform some logic only once every second to save on performance and also to stop zombies jumping right the frame after the player does

    float logic_frame_time;

    // interval on which ranged enemies attack

    float attack_time;

    float speed;
    float time_til_next_jump_impulse;
    float jump_height;

    vec2 last_used_tile_index;

    int last_target_tile_x;

    float horizontal_goal_x;
    bool has_horizontal_goal;

    // refers to all 3 possible states for where the player might be
    int pos_state_relative_to_player;

    bool enabled;
} zombie;

extern zombie zombie_pool[MAX_ZOMBIES];

void spawn_zombie(int tier, int hit_points, float speed, float jump_height);
void zombie_coreloop(entity* plyr);
void reset_zombie_progress();