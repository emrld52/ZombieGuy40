#pragma once

// libs

#include "../deps/cglm/cglm.h"

// source

#include "r_renderfuncs.h"
#include "s_animation.h"

#define MAX_COLLIDING_ENTITIES 24

typedef struct entity_t {
    vec2 position;
    vec2 velocity;

    vec2 hit_box;
    vec2 hit_box_offset;
    bool is_grounded;
    float gravity;

    bool collision_enabled;

    sprite sprite_data;
    animator animator_component;

    int health_points;
    int max_health_points;
    int damage;
    int team;

    int id;

    bool handle_x_for_me;

    // with level geometry

    bool is_colliding;

    bool is_projectile;

    struct entity_t *colliding_entities[MAX_COLLIDING_ENTITIES];

    struct entity_t *ignore_collision_with[MAX_COLLIDING_ENTITIES / 2];
    struct entity_t *i_am_in_ignore_lists[MAX_COLLIDING_ENTITIES / 2];

    // use for whatever timed purpose

    float entity_timer;

    bool enabled;

    // used for stuff like dead bodies, we dont mind if they get overwritten but we do want them to be enabled so the bodies linger for a bit

    bool marked_for_garbage_collection;
} entity;

void entity_run_physics(entity* ent);
void entity_override_velocity(entity* ent, vec2 vel);
void add_to_entities_collision_ignore_list(entity* to_add_to, entity* adding);