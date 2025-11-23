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
#include "s_sound.h"
#include "z_upgrades.h"

// libraries

#include "../deps/cglm/cglm.h"
#include "../deps/sokol_app.h"
#include <stdbool.h>
#include <string.h>

// define player object

player ply;

entity* get_player()
{
    return ply.plyr;
}

float time_til_next_can_shoot;

int player_upgrade_rerolls = DEAFULT_REROLLS;

void reset_player()
{
    // copy lots of vector data over into player info

    glm_vec3_copy((vec2){ (VIRTUAL_WIDTH/ 2) - 32, 0.0f }, ply.plyr->position);
    glm_vec3_copy((vec2){ 0.0f, 0.0f }, ply.plyr->velocity);
    glm_vec2_copy((vec2){ 32, 32 }, ply.plyr->sprite_data.resolution);
    glm_vec2_copy((vec2){ 1, 1 }, ply.plyr->sprite_data.sprite_coord);
    glm_vec2_copy((vec2){16, ply.plyr->sprite_data.resolution[1]}, ply.plyr->hit_box);
    glm_vec2_copy((vec2){8, 0}, ply.plyr->hit_box_offset);
    ply.plyr->gravity = PLAYER_GRAVITY;

    animator_init(&ply.plyr->animator_component);
    play_animation(&ply.plyr->animator_component, &ANIM_PLAYER_IDLE);

    // player team is 1, zombies are on team 0. entities on the same team dont check collision with one another so this is needed

    ply.plyr->team = 1;
    ply.plyr->damage = 0;

    // init stats and such from here

    ply.plyr->health_points = 2;
    ply.plyr->max_health_points = 2;

    ply.plyr->id = PLAYER_ID;

    init_hp_ui(ply.plyr);

    ply.player_bullet_type = &REGULAR_BULLETS;
    ply.wpn.fire_rate = 0.33f;
    ply.wpn.is_auto = false;

    ply.fire_both_directions = false;

    ply.invinc_time = PLAYER_INVINCIBILITY_TIME_AFTER_HIT;
    ply.plyr->entity_timer = 0;

    player_upgrade_rerolls = DEAFULT_REROLLS;

    ply.bullet_overrides.bounces = 0;
    ply.bullet_overrides.damage = 0;
    ply.bullet_overrides.pierces = 0;
}

// init player position and velocity

void player_init() {
    ply.plyr = make_entity_in_scene(loaded_scene);

    reset_player();
}

void fire_gun() {
    if(!ply.fire_both_directions) {
        bullet* b = make_bullet(ply.player_bullet_type, ply.plyr->position, global_input.mouse_x + 16 >= ply.plyr->position[0] ? 1 : -1, ply.plyr->team);
        b->entity->damage += ply.bullet_overrides.damage;
        b->bounces_left += ply.bullet_overrides.bounces;
        b->pierces_left +=ply.bullet_overrides.pierces;
    }
    else {
        bullet* b1 = make_bullet(ply.player_bullet_type, ply.plyr->position, 1, ply.plyr->team);
        b1->entity->damage += ply.bullet_overrides.damage;
        b1->bounces_left += ply.bullet_overrides.bounces;
        b1->pierces_left +=ply.bullet_overrides.pierces;
        bullet* b2 = make_bullet(ply.player_bullet_type, ply.plyr->position, -1, ply.plyr->team);
        b2->entity->damage += ply.bullet_overrides.damage;
        b2->bounces_left += ply.bullet_overrides.bounces;
        b2->pierces_left +=ply.bullet_overrides.pierces;
    }

    camera_shake(5.0f);
    time_til_next_can_shoot = ply.wpn.fire_rate;
}

void damage_player(entity* attacker) {
    play_override_animation(&ply.plyr->animator_component, ANIM_PLAYER_DAMAGE);
    ply.plyr->entity_timer = ply.invinc_time;
    ply.plyr->velocity[0] = attacker->position[0] >= ply.plyr->position[0] ? -PLAYER_KNOCKBACK_STRENGTH_X : PLAYER_KNOCKBACK_STRENGTH_X;
    ply.plyr->velocity[1] = -PLAYER_KNOCKBACK_STRENGTH_Y;
    //ply.plyr->collision_enabled = false;
    ply.plyr->health_points -= attacker->damage;
    camera_shake(15.0f);
    damage_ui_hp(ply.plyr);
    play_sound("hit.wav");

    if(ply.plyr->health_points <= 0) 
    {
        play_sound("player_die.wav");
        ply.plyr->damage = 0;
    }
}

void player_loop() {
    if(!is_paused && !force_paused) {
        if(ply.plyr->health_points >= 1) {
            // basic movement, can only move if not stunned (tracked in entity timer)

            if(ply.plyr->entity_timer <= ply.invinc_time - PLAYER_STUN_THRESHOLD) {

                if(global_input.keys_pressed[SAPP_KEYCODE_D]) ply.plyr->velocity[0] = PLAYER_MAX_SPEED;
                else if(global_input.keys_pressed[SAPP_KEYCODE_A]) ply.plyr->velocity[0] = -PLAYER_MAX_SPEED;
                else ply.plyr->velocity[0] = 0;
            }

            // check if colliding with any damaging entities

            for(int i = 0; i < MAX_COLLIDING_ENTITIES; i++)
            {
                if(ply.plyr->colliding_entities[i] != NULL && ply.plyr->entity_timer <= 0 
                    && ply.plyr->colliding_entities[i]->collision_enabled && ply.plyr->colliding_entities[i]->team != ply.plyr->team) 
                {
                    if(ply.plyr->colliding_entities[i]->damage >= 1) damage_player(ply.plyr->colliding_entities[i]);
                }
            }

            // animation states and jump logic

            if(ply.plyr->is_grounded) 
            {
                if(global_input.keys_pressed[SAPP_KEYCODE_SPACE] || global_input.keys_pressed[SAPP_KEYCODE_W]) 
                {
                    entity_override_velocity(ply.plyr, (vec2){ply.plyr->velocity[0], -PLAYER_JUMP_FORCE});
                    camera_shake(2.0f);
                    play_sound("jump.wav");
                }

                if(ply.plyr->velocity[0] == 0) play_animation(&ply.plyr->animator_component, &ANIM_PLAYER_IDLE);
                else if(ply.plyr->is_grounded) play_animation(&ply.plyr->animator_component, &ANIM_PLAYER_RUN);
            }
            else 
            {
                play_animation(&ply.plyr->animator_component, &ANIM_PLAYER_JUMP);

                // variable jump height

                if((global_input.keys_released[SAPP_KEYCODE_SPACE] || global_input.keys_released[SAPP_KEYCODE_W]) && ply.plyr->velocity[1] < 0) 
                    entity_override_velocity(ply.plyr, (vec2){ply.plyr->velocity[0], ply.plyr->velocity[1] / PLAYER_JUMP_CANCEL_STRENGTH});
            }

            // reload
            time_til_next_can_shoot -= global_delta_time * loaded_scene->scene_game_speed;

            // enable player collision upon invinc frames being done

            if(ply.plyr->entity_timer <= 0) ply.plyr->collision_enabled = true;

            // check for shooting, if auto you can hold to shoot, otherwise its manual click

            if(ply.wpn.is_auto && global_input.mouse_l && time_til_next_can_shoot <= 0) fire_gun();
            else if(global_input.mouse_l_up && time_til_next_can_shoot <= 0) fire_gun();

            // look right or left dependent on where the mouse is of the player center

            if(global_input.mouse_x >= ply.plyr->position[0] + (ply.plyr->hit_box[0] / 2) + ply.plyr->hit_box_offset[0]) ply.plyr->sprite_data.flip_x = false;
            else ply.plyr->sprite_data.flip_x = true;

            draw_player_stats(ply.player_bullet_type->damage + ply.bullet_overrides.damage, (int)(1.0f / ply.wpn.fire_rate), 
                ply.wpn.is_auto, ply.bullet_overrides.pierces, ply.bullet_overrides.bounces, zombies_killed_total);
        }
        // if dead
        else
        {
            ply.plyr->handle_x_for_me = true;
            play_animation(&ply.plyr->animator_component, &ANIM_ZOMBIE_DEAD1);
        }
    }
}