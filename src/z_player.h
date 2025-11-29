#pragma once

#include "r_renderfuncs.h"
#include "s_entities.h"
#include "s_weapons.h"

#define PLAYER_ID 4096

// player-movmement variables

#define PLAYER_MAX_SPEED 256.0f * 0.75f
#define PLAYER_GRAVITY 1024.0f * 1.5f
#define PLAYER_JUMP_FORCE 512.0f

#define PLAYER_INVINCIBILITY_TIME_AFTER_HIT 0.65f
#define PLAYER_STUN_THRESHOLD 0.2f

#define PLAYER_KNOCKBACK_STRENGTH_Y 200.0f
#define PLAYER_KNOCKBACK_STRENGTH_X 100.0f

#define PLAYER_JUMP_CANCEL_STRENGTH 4
#define PLAYER_FORCE_FALL_STRENGTH 1024.0f * 1.5f

#define DEBUG_INVINCIBILITY false

typedef struct player_weapon_t
{
    bool is_auto;

    // in time between each bullet
    float fire_rate;
    int damage_to_append_to_bullets;
    float grav_to_append_to_bullets;
    int pierces_to_append_to_bullets;

    sprite weapon_sprite;
} player_weapon;

typedef struct player_t {
    entity *plyr;
    player_weapon wpn;
    bullet_type *player_bullet_type;

    bool fire_both_directions;

    bullet_type_overrides bullet_overrides;

    float invinc_time;

    bool cancelled_jump;
} player;

extern player ply;

extern int player_upgrade_rerolls;

void player_init();
void player_loop();
void reset_player();
entity* get_player();