#pragma once

#include "r_renderfuncs.h"
#include "s_entities.h"

// player-movmement variables

#define PLAYER_MAX_SPEED 256.0f * 0.75f
#define PLAYER_GRAVITY 1024.0f * 1.5f
#define PLAYER_JUMP_FORCE 512.0f

#define PLAYER_INVINCIBILITY_TIME_AFTER_HIT 0.65f
#define PLAYER_STUN_THRESHOLD 0.4f

#define PLAYER_KNOCKBACK_STRENGTH_Y 200.0f
#define PLAYER_KNOCKBACK_STRENGTH_X 100.0f

#define PLAYER_JUMP_CANCEL_STRENGTH 4

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

extern player_weapon PLY_WEAPON_HANDGUN;
extern player_weapon PLY_WEAPON_ASSAULT_RIFLE;
extern player_weapon PLY_WEAPON_SNIPER;
extern player_weapon PLY_WEAPON_FLAMETHROWER;

void player_init();
void player_loop();
entity* get_player();