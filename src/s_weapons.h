#pragma once

#include "s_animation.h"
#include "s_entities.h"
#include "g_state.h"

#include "../deps/cglm/cglm.h"

#define MAX_BULLETS 96

typedef struct bullet_type_t {
    int damage;
    float speed;
    float gravity;

    vec2 hit_box;
    vec2 hit_box_offset;

    animation muzzle_flash_animation;
    animation bullet_zoom_animation;

    int pierce_count;
} bullet_type;

typedef struct weapon_t {
    bullet_type weapon_bullets;

    // in bullets per sec

    float fire_speed;

    float kickback;
} weapon;

typedef struct bullet_t {
    bullet_type *type;
    entity* entity;
    bool enabled;
    int pierces_left;
} bullet;

extern bullet_type REGULAR_BULLETS;
extern bullet_type PIERCING_BULLETS;
extern bullet_type ENEMY_BULLETS;
extern bullet_type DOUBLE_PIERCING_BULLETS;

extern bullet bullet_object_pool[MAX_BULLETS];

void init_weapon_system();
void bullets_update();
void bullet_garbage_collection();
bullet *make_bullet(bullet_type *typ, vec2 pos, int dir, int team);