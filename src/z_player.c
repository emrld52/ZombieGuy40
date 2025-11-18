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

// player-movmement variables

#define PLAYER_MAX_SPEED 256.0f * 0.75f
#define PLAYER_GRAVITY 1024.0f * 1.5f
#define PLAYER_JUMP_FORCE 512.0f

entity *ply;

entity* get_player()
{
    return ply;
}

bullet b;

float time_til_next_can_shoot = 0.25f;

float reload_time = 0.33f;
bool auto_gun = false;
bool piercing_rounds = false;
bool double_piercing_rounds = false;

// init player position and velocity

void player_init()
{
    ply = make_entity_in_scene(loaded_scene);

    glm_vec3_copy((vec2){ (VIRTUAL_WIDTH/ 2) - 32, 0.0f }, ply->position);
    glm_vec3_copy((vec2){ 0.0f, 0.0f }, ply->velocity);
    glm_vec2_copy((vec2){ 32, 32 }, ply->sprite_data.resolution);
    glm_vec2_copy((vec2){ 1, 1 }, ply->sprite_data.sprite_coord);
    glm_vec2_copy((vec2){16, ply->sprite_data.resolution[1]}, ply->hit_box);
    glm_vec2_copy((vec2){8, 0}, ply->hit_box_offset);
    ply->gravity = PLAYER_GRAVITY;

    animator_init(&ply->animator_component);
    play_animation(&ply->animator_component, &ANIM_PLAYER_IDLE);

    ply->team = 1;

    ply->health_points = 3;
    ply->max_health_points = 3;

    reload_time = 0.33f;
    auto_gun = false;
    piercing_rounds = false;
    double_piercing_rounds = true;
}

void player_loop()
{
    if(ply->health_points >= 1) {
        // invincibility frames on damage
        ply->entity_timer -= global_delta_time * loaded_scene->scene_game_speed;
        time_til_next_can_shoot -= global_delta_time * loaded_scene->scene_game_speed;

        if(ply->entity_timer <= 0) ply->collision_enabled = true;

        //printf("%d", &ply->position[1]);

        for(int i = 0; i < MAX_COLLIDING_ENTITIES; i++)
        {
            // test
            if(ply->colliding_entities[i] != NULL && ply->entity_timer <= 0 && ply->colliding_entities[i]->team != ply->team) 
            {
                // projectiles disable too early and behave weirdly, this fixes it
                if(ply->colliding_entities[i]->is_projectile)
                {
                    play_override_animation(&ply->animator_component, ANIM_PLAYER_DAMAGE);
                    //ply->entity_timer = PLAYER_INVINCIBILITY_TIME_AFTER_HIT;
                    ply->velocity[0] = ply->colliding_entities[i]->position[0] >= ply->position[0] ? -100.0f : 100.0f;
                    ply->velocity[1] = -200.0f;
                    ply->collision_enabled = false;
                    //ply->health_points -= ply->colliding_entities[i]->damage;
                    camera_shake(15.0f);
                    damage_ui_hp(ply);
                }
                else if(ply->colliding_entities[i]->collision_enabled) {
                    if(ply->colliding_entities[i]->damage >= 1) {
                        play_override_animation(&ply->animator_component, ANIM_PLAYER_DAMAGE);
                        //ply->entity_timer = PLAYER_INVINCIBILITY_TIME_AFTER_HIT;
                        ply->velocity[0] = ply->colliding_entities[i]->position[0] >= ply->position[0] ? -100.0f : 100.0f;
                        ply->velocity[1] = -200.0f;
                        ply->collision_enabled = false;
                        //ply->health_points -= ply->colliding_entities[i]->damage;
                        camera_shake(15.0f);
                        damage_ui_hp(ply);
                    }
                    // crate stuff, hardcoded here for now
                    else if(ply->colliding_entities[i]->id == 1000)
                    {
                        destroy_crate();

                        int upgrade = rand() % 6;

                        switch(upgrade)
                        {
                            case 0:
                                reload_time /= 1.15f;
                                auto_gun = true;
                                break;

                            case 1:
                                reload_time /= 1.15f;
                                break;

                            case 2:
                                if(piercing_rounds) double_piercing_rounds = true;
                                piercing_rounds = true;
                                break;
                            case 3:
                                ply->health_points += 1;
                                if(ply->health_points >= ply->max_health_points) ply->health_points = ply->max_health_points;
                                else heal_ui_hp(ply);
                                break;
                            case 4:
                                ply->health_points += 1;
                                if(ply->health_points >= ply->max_health_points) ply->health_points = ply->max_health_points;
                                else heal_ui_hp(ply);
                                break;
                            case 5:
                                ply->max_health_points += 1;
                                ply->health_points += 1;
                                init_hp_ui(ply);
                                break;
                        }
                    }
                }
            }
        }

        // animation states and jump logic

        if(ply->is_grounded) 
        {
            if(global_input.keys_pressed[SAPP_KEYCODE_SPACE]) 
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

            if(global_input.keys_released[SAPP_KEYCODE_SPACE] && ply->velocity[1] < 0) entity_override_velocity(ply, (vec2){ply->velocity[0], ply->velocity[1] / 4});
        }

        if(!auto_gun) {
            if(global_input.mouse_l_up && time_til_next_can_shoot <= 0) 
            {
                if(double_piercing_rounds) b = *make_bullet(&DOUBLE_PIERCING_BULLETS, ply->position, global_input.mouse_x + 16 >= ply->position[0] ? 1 : -1, ply->team);
                else if(piercing_rounds) *make_bullet(&PIERCING_BULLETS, ply->position, global_input.mouse_x + 16 >= ply->position[0] ? 1 : -1, ply->team);
                else *make_bullet(&REGULAR_BULLETS, ply->position, global_input.mouse_x + 16 >= ply->position[0] ? 1 : -1, ply->team);
                if(&b != NULL) camera_shake(5.0f);
                time_til_next_can_shoot = reload_time;
            }
        }
        else
        {
            if(global_input.mouse_l && time_til_next_can_shoot <= 0) 
            {
                if(double_piercing_rounds) b = *make_bullet(&DOUBLE_PIERCING_BULLETS, ply->position, global_input.mouse_x + 16 >= ply->position[0] ? 1 : -1, ply->team);
                else if(piercing_rounds) *make_bullet(&PIERCING_BULLETS, ply->position, global_input.mouse_x + 16 >= ply->position[0] ? 1 : -1, ply->team);
                else *make_bullet(&REGULAR_BULLETS, ply->position, global_input.mouse_x + 16 >= ply->position[0] ? 1 : -1, ply->team);
                if(&b != NULL) camera_shake(5.0f);
                time_til_next_can_shoot = reload_time;
            }
        }

        // debug

        /*if(global_input.keys_released[SAPP_KEYCODE_F]) 
        {
            ply->max_health_points += 1;
            ply->health_points = ply->max_health_points;
            init_hp_ui(ply);
        }*/

        // debug

        int count = 0;

        for(int i = 0; i < MAX_COLLIDING_ENTITIES; i++)
        {
            if(ply->i_am_in_ignore_lists[i] != NULL) count += 1;
        }

        int count2 = 0;

        for(int i = 0; i < MAX_COLLIDING_ENTITIES; i++)
        {
            if(ply->colliding_entities[i] != NULL) count2 += 1;
        }

        int count3 = 0;

        for(int i = 0; i < MAX_COLLIDING_ENTITIES; i++)
        {
            if(ply->ignore_collision_with[i] != NULL) count3 += 1;
        }

        printf("\nbeing ignored by : %d colliding with : %d ignoring : %d", count, count2, count3);

        // debug tilemap builder

        /*if(global_input.keys_pressed[SAPP_KEYCODE_1] && global_input.mouse_x > 0 && global_input.mouse_y > 0)
        {
            if(!loaded_scene->tilemap[(int)floor(global_input.mouse_y / LEVELS_TILE_RESOLUTION)][(int)floor(global_input.mouse_x / LEVELS_TILE_RESOLUTION)].is_filled)
            {
                loaded_scene->tilemap[(int)floor(global_input.mouse_y / LEVELS_TILE_RESOLUTION)][(int)floor(global_input.mouse_x / LEVELS_TILE_RESOLUTION)].is_filled = true;
                autotiler_build_tilemap(loaded_scene->tilemap);
                camera_shake(1.0f);
            }
        }
        if(global_input.keys_pressed[SAPP_KEYCODE_2] && global_input.mouse_x > 0 && global_input.mouse_y > 0)
        {
            if(loaded_scene->tilemap[(int)floor(global_input.mouse_y / LEVELS_TILE_RESOLUTION)][(int)floor(global_input.mouse_x / LEVELS_TILE_RESOLUTION)].is_filled)
            {
                loaded_scene->tilemap[(int)floor(global_input.mouse_y / LEVELS_TILE_RESOLUTION)][(int)floor(global_input.mouse_x / LEVELS_TILE_RESOLUTION)].is_filled = false;
                autotiler_build_tilemap(loaded_scene->tilemap);
                camera_shake(1.0f);
            }
        }*/

        // basic movement

        if(ply->entity_timer <= 0.4f) {

            if(global_input.keys_pressed[SAPP_KEYCODE_D]) ply->velocity[0] = PLAYER_MAX_SPEED;
            else if(global_input.keys_pressed[SAPP_KEYCODE_A]) ply->velocity[0] = -PLAYER_MAX_SPEED;
            else ply->velocity[0] = 0;

        }

        // look right or left dependent on where the mouse is of the player center

        if(global_input.mouse_x >= ply->position[0] + (ply->hit_box[0] / 2) + ply->hit_box_offset[0]) ply->sprite_data.flip_x = false;
        else ply->sprite_data.flip_x = true;
    }
    else
    {
        ply->handle_x_for_me = true;
        play_animation(&ply->animator_component, &ANIM_ZOMBIE_DEAD1);
    }
}

void player_reset()
{

}