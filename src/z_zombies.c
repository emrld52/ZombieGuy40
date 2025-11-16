#include "z_zombies.h"

// libs

#include "../deps/cglm/cglm.h"
#include <stdbool.h>
#include <stdlib.h>

// game logic

#include "g_state.h"
#include "z_coreloop.h"
#include "z_player.h"
#include "s_weapons.h"

// rendering

#include "r_renderfuncs.h"

#define ZOMBIE_GRAV 1024.0f

// timer

float power_timer = 0.0f;

float time_between_each_zombie = STARTING_ZOMBIE_SPAWN_TIME;
float time_til_next_zombie = 0;

// object pooling

zombie zombie_pool[MAX_ZOMBIES];

// spawn zombie function, make new zombie out of object pool with passable info

void spawn_zombie(int tier, int hit_points, float speed, float jump_height)
{
    printf("bazinga!");
    // linear search through pool to find zombie to spawn, avoids dynamic memory allocation which is probably out of scope for a project this size

    for(int i = 0; i < MAX_ZOMBIES; i++)
    {
        // spawn first inactive zombie in pool

        if(!zombie_pool[i].enabled)
        {
            printf("bazinga!");

            zombie_pool[i].zmb = make_entity_in_scene(loaded_scene);
            play_animation(&zombie_pool[i].zmb->animator_component, &ANIM_MINION_IDLE);

            // spawn in sky at any horizontal position in game-space. will fall down

            zombie_pool[i].zmb->position[0] = rand() % VIRTUAL_WIDTH;
            zombie_pool[i].zmb->position[1] = -64;

            zombie_pool[i].zmb->gravity = ZOMBIE_GRAV;

            glm_vec3_copy((vec2){ 0.0f, 0.0f }, zombie_pool[i].zmb->velocity);
            glm_vec2_copy((vec2){ 32, 32 }, zombie_pool[i].zmb->sprite_data.resolution);
            glm_vec2_copy((vec2){ 1, 1 }, zombie_pool[i].zmb->sprite_data.sprite_coord);
            glm_vec2_copy((vec2){16, zombie_pool[i].zmb->sprite_data.resolution[1]}, zombie_pool[i].zmb->hit_box);
            glm_vec2_copy((vec2){8, 0}, zombie_pool[i].zmb->hit_box_offset);

            zombie_pool[i].zmb->velocity[0] = get_player()->position[0] >= zombie_pool[i].zmb->position[0] ? speed : -speed;

            zombie_pool[i].tier = tier;
            zombie_pool[i].speed = speed;
            zombie_pool[i].jump_height = jump_height;

            zombie_pool[i].zmb->team = 0;

            zombie_pool[i].zmb->health_points = hit_points;

            zombie_pool[i].time_til_next_jump_impulse = 0;

            zombie_pool[i].enabled = true;

            zombie_pool[i].logic_frame_time = rand() % (1000 - 0 + 1) + 1;;

            // convert from ms to seconds
            zombie_pool[i].logic_frame_time /= 1000;

            return;
        }
    }
}

void kill_zombie(zombie *zomb)
{
    zomb->enabled = false;

    zomb->zmb->marked_for_garbage_collection = true;

    // randomize corpse

    if(rand() % 2 == 1) play_animation(&zomb->zmb->animator_component, &ANIM_ZOMBIE_DEAD1);
    else play_animation(&zomb->zmb->animator_component, &ANIM_ZOMBIE_DEAD2);

    zomb->zmb->collision_enabled = false;
    zomb->zmb->handle_x_for_me = true;

    if(zomb->zmb->entity_timer <= 0) zomb->zmb->velocity[0] = 0;

    zomb->zmb = NULL;
}

void damage_zombie(zombie *zomb, entity *attacker)
{
    play_override_animation(&zomb->zmb->animator_component, ANIM_ZOMBIE_DAMAGE);
    zomb->zmb->entity_timer = 0.35f;
    zomb->zmb->velocity[0] = attacker->position[0] >= zomb->zmb->position[0] ? -100.0f : 100.0f;
    zomb->zmb->velocity[1] = -200.0f;
    zomb->zmb->collision_enabled = false;
    camera_shake(2.0f);
    zomb->zmb->health_points -= attacker->damage;

    if(zomb->zmb->health_points <= 0) kill_zombie(zomb);
}

// this pathfinding is VERY primitive and relies on the level being mostly symmetrical with only a tiny bit of verticality. however it does work well enough for what were trying

void minion_ai(zombie *zomb, entity *plyr)
{
    if(zomb->enabled) 
    {
        zomb->logic_frame_time += global_delta_time * loaded_scene->scene_game_speed;
        zomb->zmb->entity_timer -= global_delta_time * loaded_scene->scene_game_speed;

        if(zomb->zmb->health_points >= 1) {
            if(zomb->zmb->entity_timer <= 0) {
                if(zomb->zmb->is_grounded) play_animation(&zomb->zmb->animator_component, &ANIM_MINION_RUN);
                else play_animation(&zomb->zmb->animator_component, &ANIM_MINION_JUMP);

                if(zomb->zmb->velocity[0] >= 0) zomb->zmb->sprite_data.flip_x = false;
                else zomb->zmb->sprite_data.flip_x = true;

                if(zomb->logic_frame_time >= 0.2f) 
                {
                    zomb->logic_frame_time = 0.0f;

                    // if player is on level with enemy. add some accounting for potential collision stuff by adding a little to the y value. use tile coordinates to be more exact.
                    if((int)((plyr->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION) == (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION))
                    {
                        // follow player

                        if(plyr->position[0] > zomb->zmb->position[0]) zomb->zmb->velocity[0] = zomb->speed;
                        else if(plyr->position[0] < zomb->zmb->position[0]) zomb->zmb->velocity[0] = -zomb->speed;
                        else zomb->zmb->velocity[0] = 0;

                        for(int i = 0; i < LEVELS_WIDTH; i++)
                        {
                            //printf("\n%d", (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION));
                            if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][i].direction_for_pathing == 2)
                            {
                                printf("%d", (int)((zomb->zmb->position[0] + 0.1f) / LEVELS_TILE_RESOLUTION));
                            }
                        }

                        if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][(int)((zomb->zmb->position[0] + 0.1f) / LEVELS_TILE_RESOLUTION)].direction_for_pathing == 2
                            && zomb->zmb->is_grounded)
                        {
                            entity_override_velocity(zomb->zmb, (vec2){zomb->zmb->velocity[0] * 1.4f, -zomb->jump_height});
                        }
                    }
                    else if((int)((plyr->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION) < (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION))
                    {
                        int jump_tiles_near_me[2] = {-1, -1};
                        bool has_found_tile = false;

                        // jump_tiles_near_me stores jumping tile coordinates in the row this zombie is in

                        for(int i = 0; i < LEVELS_WIDTH; i++)
                        {
                            //printf("\n%d", (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION));
                            if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][i].direction_for_pathing >= 2)
                            {
                                if(jump_tiles_near_me[0] == -1) 
                                {
                                    jump_tiles_near_me[0] = i;
                                    has_found_tile = true;
                                }
                                else jump_tiles_near_me[1] = i;
                            }
                        }

                        if(has_found_tile) {
                            //printf("\ni found up tiles! x %d and x %d", jump_tiles_near_me[0], jump_tiles_near_me[1]);
                            // decide which is closer to begin moving to

                            int dist_from_tile_1 = glm_vec2_distance2((vec2){jump_tiles_near_me[0] * LEVELS_TILE_RESOLUTION, zomb->zmb->position[1]}, zomb->zmb->position);
                            int dist_from_tile_2 = glm_vec2_distance2((vec2){jump_tiles_near_me[1] * LEVELS_TILE_RESOLUTION, zomb->zmb->position[1]}, zomb->zmb->position);

                            if(dist_from_tile_1 >= dist_from_tile_2) zomb->zmb->velocity[0] = zomb->speed;
                            else zomb->zmb->velocity[0] = -zomb->speed;
                        }

                        if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][(int)((zomb->zmb->position[0] + 0.1f) / LEVELS_TILE_RESOLUTION)].direction_for_pathing == 2
                            && zomb->zmb->is_grounded)
                        {
                            entity_override_velocity(zomb->zmb, (vec2){zomb->zmb->velocity[0] * 1.4f, -zomb->jump_height});
                        }
                        else if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][(int)((zomb->zmb->position[0] + 0.1f) / LEVELS_TILE_RESOLUTION)].direction_for_pathing == 3
                            && zomb->zmb->is_grounded)
                        {
                            entity_override_velocity(zomb->zmb, (vec2){zomb->speed*2, -zomb->jump_height*1.25f});
                        }
                        else if (loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][(int)((zomb->zmb->position[0] + 0.1f) / LEVELS_TILE_RESOLUTION)].direction_for_pathing == 4
                            && zomb->zmb->is_grounded)
                        {
                            entity_override_velocity(zomb->zmb, (vec2){-zomb->speed*2, -zomb->jump_height*1.25f});
                        }
                        
                    }
                    else
                    {
                        int drop_tiles_near_me[2] = {-1, -1};
                        bool has_found_tile = false;

                        // jump_tiles_near_me stores jumping tile coordinates in the row this zombie is in

                        for(int i = 0; i < LEVELS_WIDTH; i++)
                        {
                            //printf("\n%d", (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION));
                            if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][i].direction_for_pathing == 1)
                            {
                                if(drop_tiles_near_me[0] == -1) 
                                {
                                    drop_tiles_near_me[0] = i;
                                    has_found_tile = true;
                                }
                                else drop_tiles_near_me[1] = i;
                            }
                        }

                        if(has_found_tile) {
                            printf("\ni found down tiles! x %d and x %d", drop_tiles_near_me[0], drop_tiles_near_me[1]);
                            // decide which is closer to begin moving to

                            int dist_from_tile_1 = glm_vec2_distance2((vec2){drop_tiles_near_me[0] * LEVELS_TILE_RESOLUTION, zomb->zmb->position[1]}, zomb->zmb->position);
                            int dist_from_tile_2 = glm_vec2_distance2((vec2){drop_tiles_near_me[1] * LEVELS_TILE_RESOLUTION, zomb->zmb->position[1]}, zomb->zmb->position);

                            if(dist_from_tile_1 >= dist_from_tile_2) zomb->zmb->velocity[0] = -zomb->speed;
                            else zomb->zmb->velocity[0] = zomb->speed;
                        }
                    }
                }
            }
        }

        if(zomb->zmb->entity_timer <= 0) zomb->zmb->collision_enabled = true;

        for(int i = 0; i < MAX_COLLIDING_ENTITIES; i++)
        {
            // test
            if(zomb->zmb->colliding_entities[i] != NULL && zomb->zmb->entity_timer <= 0 
                && zomb->zmb->colliding_entities[i]->damage >= 1 && zomb->zmb->colliding_entities[i]->team != zomb->zmb->team
                && zomb->zmb->health_points >= 1) 
            {
                damage_zombie(zomb, zomb->zmb->colliding_entities[i]);
            }
        }
    }
}

void ranger_ai(zombie *zomb, entity *plyr)
{
    if(zomb->enabled) 
    {
        zomb->logic_frame_time += global_delta_time * loaded_scene->scene_game_speed;
        zomb->zmb->entity_timer -= global_delta_time * loaded_scene->scene_game_speed;

        if(zomb->zmb->health_points >= 1) {
            if(zomb->zmb->entity_timer <= 0) {
                if(zomb->zmb->is_grounded) 
                {
                    if(zomb->zmb->velocity[0] != 0) play_animation(&zomb->zmb->animator_component, &ANIM_RANGER_RUN);
                    else play_animation(&zomb->zmb->animator_component, &ANIM_RANGER_IDLE);
                }
                else play_animation(&zomb->zmb->animator_component, &ANIM_RANGER_JUMP);

                zomb->zmb->sprite_data.flip_x = plyr->position[0] >= zomb->zmb->position[0] ? false : true;

                if(zomb->logic_frame_time >= 2.0f) 
                {
                    zomb->logic_frame_time = 0.0f;

                    make_bullet(&REGULAR_BULLETS, zomb->zmb->position, plyr->position[0] >= zomb->zmb->position[0] ? 1 : -1, 0);

                    // if player is on level with enemy. add some accounting for potential collision stuff by adding a little to the y value. use tile coordinates to be more exact.
                    if((int)((plyr->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION) == (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION))
                    {
                        zomb->zmb->velocity[0] = 0;
                    }
                    else if((int)((plyr->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION) < (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION))
                    {
                        int jump_tiles_near_me[2] = {-1, -1};
                        bool has_found_tile = false;

                        // jump_tiles_near_me stores jumping tile coordinates in the row this zombie is in

                        for(int i = 0; i < LEVELS_WIDTH; i++)
                        {
                            //printf("\n%d", (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION));
                            if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][i].direction_for_pathing >= 2)
                            {
                                if(jump_tiles_near_me[0] == -1) 
                                {
                                    jump_tiles_near_me[0] = i;
                                    has_found_tile = true;
                                }
                                else jump_tiles_near_me[1] = i;
                            }
                        }

                        if(has_found_tile) {
                            //printf("\ni found up tiles! x %d and x %d", jump_tiles_near_me[0], jump_tiles_near_me[1]);
                            // decide which is closer to begin moving to

                            int dist_from_tile_1 = glm_vec2_distance2((vec2){jump_tiles_near_me[0] * LEVELS_TILE_RESOLUTION, zomb->zmb->position[1]}, zomb->zmb->position);
                            int dist_from_tile_2 = glm_vec2_distance2((vec2){jump_tiles_near_me[1] * LEVELS_TILE_RESOLUTION, zomb->zmb->position[1]}, zomb->zmb->position);

                            if(dist_from_tile_1 >= dist_from_tile_2) zomb->zmb->velocity[0] = zomb->speed;
                            else zomb->zmb->velocity[0] = -zomb->speed;
                        }

                        if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][(int)((zomb->zmb->position[0] + 0.1f) / LEVELS_TILE_RESOLUTION)].direction_for_pathing == 2
                            && zomb->zmb->is_grounded)
                        {
                            entity_override_velocity(zomb->zmb, (vec2){zomb->zmb->velocity[0] * 1.4f, -zomb->jump_height});
                        }
                        else if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][(int)((zomb->zmb->position[0] + 0.1f) / LEVELS_TILE_RESOLUTION)].direction_for_pathing == 3
                            && zomb->zmb->is_grounded)
                        {
                            entity_override_velocity(zomb->zmb, (vec2){zomb->speed*2, -zomb->jump_height*1.25f});
                        }
                        else if (loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][(int)((zomb->zmb->position[0] + 0.1f) / LEVELS_TILE_RESOLUTION)].direction_for_pathing == 4
                            && zomb->zmb->is_grounded)
                        {
                            entity_override_velocity(zomb->zmb, (vec2){-zomb->speed*2, -zomb->jump_height*1.25f});
                        }
                        
                    }
                    else
                    {
                        int drop_tiles_near_me[2] = {-1, -1};
                        bool has_found_tile = false;

                        // jump_tiles_near_me stores jumping tile coordinates in the row this zombie is in

                        for(int i = 0; i < LEVELS_WIDTH; i++)
                        {
                            //printf("\n%d", (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION));
                            if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][i].direction_for_pathing == 1)
                            {
                                if(drop_tiles_near_me[0] == -1) 
                                {
                                    drop_tiles_near_me[0] = i;
                                    has_found_tile = true;
                                }
                                else drop_tiles_near_me[1] = i;
                            }
                        }

                        if(has_found_tile) {
                            printf("\ni found down tiles! x %d and x %d", drop_tiles_near_me[0], drop_tiles_near_me[1]);
                            // decide which is closer to begin moving to

                            int dist_from_tile_1 = glm_vec2_distance2((vec2){drop_tiles_near_me[0] * LEVELS_TILE_RESOLUTION, zomb->zmb->position[1]}, zomb->zmb->position);
                            int dist_from_tile_2 = glm_vec2_distance2((vec2){drop_tiles_near_me[1] * LEVELS_TILE_RESOLUTION, zomb->zmb->position[1]}, zomb->zmb->position);

                            if(dist_from_tile_1 >= dist_from_tile_2) zomb->zmb->velocity[0] = -zomb->speed;
                            else zomb->zmb->velocity[0] = zomb->speed;
                        }
                    }
                }
            }
        }

        if(zomb->zmb->entity_timer <= 0) zomb->zmb->collision_enabled = true;

        for(int i = 0; i < MAX_COLLIDING_ENTITIES; i++)
        {
            // test
            if(zomb->zmb->colliding_entities[i] != NULL && zomb->zmb->entity_timer <= 0 
                && zomb->zmb->colliding_entities[i]->damage >= 1 && zomb->zmb->colliding_entities[i]->team != zomb->zmb->team
                && zomb->zmb->health_points >= 1) 
            {
                damage_zombie(zomb, zomb->zmb->colliding_entities[i]);
            }
        }
    }
}

void king_ai(zombie *zomb, entity *plyr)
{
    if(zomb->enabled) 
    {
        zomb->logic_frame_time += global_delta_time * loaded_scene->scene_game_speed;
        zomb->zmb->entity_timer -= global_delta_time * loaded_scene->scene_game_speed;

        if(zomb->zmb->health_points >= 1) {
            if(zomb->zmb->entity_timer <= 0) {
                if(zomb->zmb->is_grounded) play_animation(&zomb->zmb->animator_component, &ANIM_MINION_RUN);
                else play_animation(&zomb->zmb->animator_component, &ANIM_MINION_JUMP);

                if(zomb->zmb->velocity[0] >= 0) zomb->zmb->sprite_data.flip_x = false;
                else zomb->zmb->sprite_data.flip_x = true;

                if(zomb->logic_frame_time >= 0.2f) 
                {
                    zomb->logic_frame_time = 0.0f;

                    // if player is on level with enemy. add some accounting for potential collision stuff by adding a little to the y value. use tile coordinates to be more exact.
                    if((int)((plyr->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION) == (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION))
                    {
                        // follow player

                        if(plyr->position[0] > zomb->zmb->position[0]) zomb->zmb->velocity[0] = zomb->speed;
                        else if(plyr->position[0] < zomb->zmb->position[0]) zomb->zmb->velocity[0] = -zomb->speed;
                        else zomb->zmb->velocity[0] = 0;

                        for(int i = 0; i < LEVELS_WIDTH; i++)
                        {
                            //printf("\n%d", (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION));
                            if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][i].direction_for_pathing == 2)
                            {
                                printf("%d", (int)((zomb->zmb->position[0] + 0.1f) / LEVELS_TILE_RESOLUTION));
                            }
                        }

                        if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][(int)((zomb->zmb->position[0] + 0.1f) / LEVELS_TILE_RESOLUTION)].direction_for_pathing == 2
                            && zomb->zmb->is_grounded)
                        {
                            entity_override_velocity(zomb->zmb, (vec2){zomb->zmb->velocity[0] * 1.4f, -zomb->jump_height});
                        }
                    }
                    else if((int)((plyr->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION) < (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION))
                    {
                        int jump_tiles_near_me[2] = {-1, -1};
                        bool has_found_tile = false;

                        // jump_tiles_near_me stores jumping tile coordinates in the row this zombie is in

                        for(int i = 0; i < LEVELS_WIDTH; i++)
                        {
                            //printf("\n%d", (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION));
                            if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][i].direction_for_pathing >= 2)
                            {
                                if(jump_tiles_near_me[0] == -1) 
                                {
                                    jump_tiles_near_me[0] = i;
                                    has_found_tile = true;
                                }
                                else jump_tiles_near_me[1] = i;
                            }
                        }

                        if(has_found_tile) {
                            //printf("\ni found up tiles! x %d and x %d", jump_tiles_near_me[0], jump_tiles_near_me[1]);
                            // decide which is closer to begin moving to

                            int dist_from_tile_1 = glm_vec2_distance2((vec2){jump_tiles_near_me[0] * LEVELS_TILE_RESOLUTION, zomb->zmb->position[1]}, zomb->zmb->position);
                            int dist_from_tile_2 = glm_vec2_distance2((vec2){jump_tiles_near_me[1] * LEVELS_TILE_RESOLUTION, zomb->zmb->position[1]}, zomb->zmb->position);

                            if(dist_from_tile_1 >= dist_from_tile_2) zomb->zmb->velocity[0] = zomb->speed;
                            else zomb->zmb->velocity[0] = -zomb->speed;
                        }

                        if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][(int)((zomb->zmb->position[0] + 0.1f) / LEVELS_TILE_RESOLUTION)].direction_for_pathing == 2
                            && zomb->zmb->is_grounded)
                        {
                            entity_override_velocity(zomb->zmb, (vec2){zomb->zmb->velocity[0] * 1.4f, -zomb->jump_height});
                        }
                        else if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][(int)((zomb->zmb->position[0] + 0.1f) / LEVELS_TILE_RESOLUTION)].direction_for_pathing == 3
                            && zomb->zmb->is_grounded)
                        {
                            entity_override_velocity(zomb->zmb, (vec2){zomb->speed*2, -zomb->jump_height*1.25f});
                        }
                        else if (loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][(int)((zomb->zmb->position[0] + 0.1f) / LEVELS_TILE_RESOLUTION)].direction_for_pathing == 4
                            && zomb->zmb->is_grounded)
                        {
                            entity_override_velocity(zomb->zmb, (vec2){-zomb->speed*2, -zomb->jump_height*1.25f});
                        }
                        
                    }
                    else
                    {
                        int drop_tiles_near_me[2] = {-1, -1};
                        bool has_found_tile = false;

                        // jump_tiles_near_me stores jumping tile coordinates in the row this zombie is in

                        for(int i = 0; i < LEVELS_WIDTH; i++)
                        {
                            //printf("\n%d", (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION));
                            if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][i].direction_for_pathing == 1)
                            {
                                if(drop_tiles_near_me[0] == -1) 
                                {
                                    drop_tiles_near_me[0] = i;
                                    has_found_tile = true;
                                }
                                else drop_tiles_near_me[1] = i;
                            }
                        }

                        if(has_found_tile) {
                            printf("\ni found down tiles! x %d and x %d", drop_tiles_near_me[0], drop_tiles_near_me[1]);
                            // decide which is closer to begin moving to

                            int dist_from_tile_1 = glm_vec2_distance2((vec2){drop_tiles_near_me[0] * LEVELS_TILE_RESOLUTION, zomb->zmb->position[1]}, zomb->zmb->position);
                            int dist_from_tile_2 = glm_vec2_distance2((vec2){drop_tiles_near_me[1] * LEVELS_TILE_RESOLUTION, zomb->zmb->position[1]}, zomb->zmb->position);

                            if(dist_from_tile_1 >= dist_from_tile_2) zomb->zmb->velocity[0] = -zomb->speed;
                            else zomb->zmb->velocity[0] = zomb->speed;
                        }
                    }
                }
            }
        }

        if(zomb->zmb->entity_timer <= 0) zomb->zmb->collision_enabled = true;

        for(int i = 0; i < MAX_COLLIDING_ENTITIES; i++)
        {
            // test
            if(zomb->zmb->colliding_entities[i] != NULL && zomb->zmb->entity_timer <= 0 
                && zomb->zmb->colliding_entities[i]->damage >= 1 && zomb->zmb->colliding_entities[i]->team != zomb->zmb->team
                && zomb->zmb->health_points >= 1) 
            {
                damage_zombie(zomb, zomb->zmb->colliding_entities[i]);
            }
        }
    }
}

void simulate_zombies(entity *player)
{
    time_til_next_zombie += global_delta_time * loaded_scene->scene_game_speed;

    printf("%f", time_til_next_zombie);

    if(time_til_next_zombie >= time_between_each_zombie)
    {
        time_between_each_zombie /= SPAWN_TIME_MULT_RATE;
        time_til_next_zombie = 0;

        for(int i = 0; i < rand() % 3; i++) spawn_zombie(1, 1, 72, 512);
    }

    for(int i = 0; i < MAX_ZOMBIES; i++)
    {
        switch (zombie_pool[i].tier)
        {
            case 1:
                ranger_ai(&zombie_pool[i], player);
                break;
            
            case 2:

                break;
        }
    }
}