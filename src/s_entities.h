#pragma once

// libs

#include "../deps/cglm/cglm.h"

// source

#include "r_renderfuncs.h"
#include "s_animation.h"

typedef struct entity_t {
    vec2 position;
    vec2 velocity;

    vec2 hit_box;
    vec2 hit_box_offset;
    bool is_grounded;
    float gravity;

    sprite sprite_data;
    animator animator_component;

    int health_points;
    int damage;
    int team;

    bool enabled;
} entity;

void entity_run_physics(entity* ent);
void entity_override_velocity(entity* ent, vec2 vel);