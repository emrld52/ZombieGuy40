#pragma once

#include "s_animation.h"
#include "s_entities.h"
#include "g_state.h"

#include "../deps/cglm/cglm.h"

#define MAX_BULLETS 96
#define COLLISION_HISTORY_LIMIT 8

#define BOUNCE_UP_STRENGTH 512.0f

#define BOUNCE_TIME_IMMUNITY 0.1f

typedef struct bullet_type_t {
    int damage;
    float speed;
    float gravity;

    vec2 hit_box;
    vec2 hit_box_offset;

    animation muzzle_flash_animation;
    animation bullet_zoom_animation;

    int pierce_count;

    int bounce_count;

    char sfx[32];
} bullet_type;

typedef struct bullet_t {
    bullet_type *type;
    entity* entity;
    bool enabled;
    int pierces_left;
    int bounces_left;
    // fixes frametime issues
    // bool destroy_me;
    entity* collision_history[COLLISION_HISTORY_LIMIT];

    vec2 stored_vel;

    float bounce_time_immunity;
} bullet;

typedef struct bullet_type_overrides_t {
    int pierces;
    int bounces;
    int damage;
    float gravity;
} bullet_type_overrides;

extern bullet_type REGULAR_BULLETS;
extern bullet_type PIERCING_BULLETS;
extern bullet_type ENEMY_BULLETS;
extern bullet_type DOUBLE_PIERCING_BULLETS;
extern bullet_type THROWING_KNIFE;

extern bullet bullet_object_pool[MAX_BULLETS];

void init_weapon_system();
void bullets_update();
bullet *make_bullet(bullet_type *typ, vec2 pos, int dir, int team);