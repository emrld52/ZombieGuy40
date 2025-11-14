#pragma once

// libs

#include "../deps/cglm/cglm.h"

// source

#include "r_renderfuncs.h"

typedef struct entity_t {
    vec2 position;
    vec2 velocity;
    vec2 hit_box;
    vec2 hit_box_offset;
    bool is_grounded;
    float gravity;
    sprite sprite_data;
    bool enabled;
} entity;

void entity_run_physics(entity* ent);
void entity_override_velocity(entity* ent, vec2 vel);