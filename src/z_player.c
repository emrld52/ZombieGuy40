// source

#include "z_player.h"
#include "r_renderfuncs.h"
#include "g_state.h"
#include "z_coreloop.h"
#include "s_entities.h"
#include "s_scene.h"
#include "s_animation.h"
#include "s_weapons.h"
#include "z_upgradecrate.h"
#include "s_playerUI.h"

// libraries

#include "../deps/cglm/cglm.h"
#include "../deps/sokol_app.h"
#include <stdbool.h>

// define player entity

entity *ply;

entity* get_player()
{
    return ply;
}

bullet_type *player_bullet_type;
bullet bullet_instance;

float time_til_next_can_shoot;

player_weapon p_weapon;

void reset_player()
{
    // copy lots of vector data over into player info

    glm_vec3_copy((vec2){ (VIRTUAL_WIDTH/ 2) - 32, 0.0f }, ply->position);
    glm_vec3_copy((vec2){ 0.0f, 0.0f }, ply->velocity);
    glm_vec2_copy((vec2){ 32, 32 }, ply->sprite_data.resolution);
    glm_vec2_copy((vec2){ 1, 1 }, ply->sprite_data.sprite_coord);
    glm_vec2_copy((vec2){16, ply->sprite_data.resolution[1]}, ply->hit_box);
    glm_vec2_copy((vec2){8, 0}, ply->hit_box_offset);
    ply->gravity = PLAYER_GRAVITY;

    animator_init(&ply->animator_component);
    play_animation(&ply->animator_component, &ANIM_PLAYER_IDLE);

    // player team is 1, zombies are on team 0. entities on the same team dont check collision with one another so this is needed

    ply->team = 1;

    // init stats and such from here

    ply->health_points = 3;
    ply->max_health_points = 3;

    ply->id = PLAYER_ID;

    init_hp_ui(ply);

    player_bullet_type = &REGULAR_BULLETS;
    p_weapon.fire_rate = 0.33f;
    p_weapon.is_auto = false;
}

// init player position and velocity

void player_init() {
    ply = make_entity_in_scene(loaded_scene);

    reset_player();
}

void fire_gun() {
    bullet* b = make_bullet(player_bullet_type, ply->position, global_input.mouse_x + 16 >= ply->position[0] ? 1 : -1, ply->team);

    // if bullet type was found, dont do anything if theres no available bullets in object pool

    if (b) {
        bullet_instance = *b;
        camera_shake(5.0f);
        time_til_next_can_shoot = p_weapon.fire_rate;
    }
}

void damage_player(entity* attacker) {
    play_override_animation(&ply->animator_component, ANIM_PLAYER_DAMAGE);
    ply->entity_timer = PLAYER_INVINCIBILITY_TIME_AFTER_HIT;
    ply->velocity[0] = attacker->position[0] >= ply->position[0] ? -PLAYER_KNOCKBACK_STRENGTH_X : PLAYER_KNOCKBACK_STRENGTH_X;
    ply->velocity[1] = -PLAYER_KNOCKBACK_STRENGTH_Y;
    ply->collision_enabled = false;
    ply->health_points -= attacker->damage;
    camera_shake(15.0f);
    damage_ui_hp(ply);
}

void player_accept_upgrade(int upgrd)
{
    switch(upgrd)
    {
        case 0:
            p_weapon.fire_rate /= 1.15f;
            p_weapon.is_auto = true;
            break;

        case 1:
            p_weapon.fire_rate /= 1.15f;
            break;

        case 2:
            if(player_bullet_type == &PIERCING_BULLETS) player_bullet_type = &DOUBLE_PIERCING_BULLETS;
            else player_bullet_type = &PIERCING_BULLETS;
            break;
        case 3:
            ply->health_points += 1;
            if(ply->health_points >= ply->max_health_points) ply->health_points = ply->max_health_points;
            init_hp_ui(ply);
            heal_ui_hp(ply);
            break;
        case 4:
            ply->health_points += 1;
            if(ply->health_points >= ply->max_health_points) ply->health_points = ply->max_health_points;
            init_hp_ui(ply);
            heal_ui_hp(ply);
            break;
        case 5:
            ply->max_health_points += 1;
            ply->health_points += 1;
            init_hp_ui(ply);
            break;
    }
}

void player_loop() {
    if(ply->health_points >= 1) {
        // basic movement, can only move if not stunned (tracked in entity timer)

        if(ply->entity_timer <= PLAYER_STUN_THRESHOLD) {

            if(global_input.keys_pressed[SAPP_KEYCODE_D]) ply->velocity[0] = PLAYER_MAX_SPEED;
            else if(global_input.keys_pressed[SAPP_KEYCODE_A]) ply->velocity[0] = -PLAYER_MAX_SPEED;
            else ply->velocity[0] = 0;
        }

        // check if colliding with any damaging entities

        for(int i = 0; i < MAX_COLLIDING_ENTITIES; i++)
        {
            if(ply->colliding_entities[i] != NULL && ply->entity_timer <= 0 
                && ply->colliding_entities[i]->collision_enabled && ply->colliding_entities[i]->team != ply->team) 
            {
                if(ply->colliding_entities[i]->damage >= 1) damage_player(ply->colliding_entities[i]);
            }
        }

        // animation states and jump logic

        if(ply->is_grounded) 
        {
            if(global_input.keys_pressed[SAPP_KEYCODE_SPACE] || global_input.keys_pressed[SAPP_KEYCODE_W]) 
            {
                entity_override_velocity(ply, (vec2){ply->velocity[0], -PLAYER_JUMP_FORCE});
                camera_shake(2.0f);
            }

            if(ply->velocity[0] == 0) play_animation(&ply->animator_component, &ANIM_PLAYER_IDLE);
            else if(ply->is_grounded) play_animation(&ply->animator_component, &ANIM_PLAYER_RUN);
        }
        else 
        {
            play_animation(&ply->animator_component, &ANIM_PLAYER_JUMP);

            // variable jump height

            if((global_input.keys_released[SAPP_KEYCODE_SPACE] || global_input.keys_released[SAPP_KEYCODE_W]) && ply->velocity[1] < 0) entity_override_velocity(ply, (vec2){ply->velocity[0], ply->velocity[1] / PLAYER_JUMP_CANCEL_STRENGTH});
        }

        // reload
        time_til_next_can_shoot -= global_delta_time * loaded_scene->scene_game_speed;

        // enable player collision upon invinc frames being done

        if(ply->entity_timer <= 0) ply->collision_enabled = true;

        // check for shooting, if auto you can hold to shoot, otherwise its manual click

        if(p_weapon.is_auto && global_input.mouse_l && time_til_next_can_shoot <= 0) fire_gun();
        else if(global_input.mouse_l_up && time_til_next_can_shoot <= 0) fire_gun();

        // look right or left dependent on where the mouse is of the player center

        if(global_input.mouse_x >= ply->position[0] + (ply->hit_box[0] / 2) + ply->hit_box_offset[0]) ply->sprite_data.flip_x = false;
        else ply->sprite_data.flip_x = true;
    }
    // if dead
    else
    {
        ply->handle_x_for_me = true;
        play_animation(&ply->animator_component, &ANIM_ZOMBIE_DEAD1);
    }
}