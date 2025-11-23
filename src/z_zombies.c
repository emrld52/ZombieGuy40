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
#include "z_upgradecrate.h"
#include "s_scene.h"
#include "s_sound.h"

// rendering

#include "r_renderfuncs.h"

#define ZOMBIE_GRAV 1024.0f

// keep track of time, we will slowly empower zombies basssed on this

float power_timer = 0.0f;

// time til each zombie spawns

float time_between_each_zombie = STARTING_ZOMBIE_SPAWN_TIME;
float time_til_next_zombie = 0;

// starting HP for each zombie

int minion_hp = MINION_BASE_HP;
int ranger_hp = RANGER_BASE_HP;
int king_hp = KING_BASE_HP;

// object pooling

zombie zombie_pool[MAX_ZOMBIES];

int current_max_zombies = STARTING_MAX_ZOMBIES;
int current_zombies = 0;

void kill_zombie(zombie *zomb)
{
    // clear collision references with dead body

    for(int i = 0; i < MAX_ENTITIES; i++)
    {
        entity* other = &loaded_scene->entities[i];
        if (!other->enabled) continue;

        for(int z = 0; z < MAX_COLLIDING_ENTITIES; z++)
        {
            if(other->colliding_entities[z] == zomb->zmb)
                other->colliding_entities[z] = NULL;
        }
    }

    zomb->enabled = false;

    // randomize corpse

    if(rand() % 2 == 1) play_animation(&zomb->zmb->animator_component, &ANIM_ZOMBIE_DEAD1);
    else play_animation(&zomb->zmb->animator_component, &ANIM_ZOMBIE_DEAD2);

    if(zomb->zmb->entity_timer <= 0) zomb->zmb->velocity[0] = 0;

    zomb->zmb->collision_enabled = false;
    zomb->zmb->marked_for_garbage_collection = true;

    // physics will lerp x velocity to 0, means bodies wont slide forever as we dont control them anymore

    zomb->zmb->handle_x_for_me = true;

    zomb->zmb->id = 400;

    zomb->zmb = NULL;

    current_zombies -= 1;

    zombies_killed_total += 1;
    zombies_killed += 1;

    play_sound("kill_zombie.wav");
}

void damage_zombie(zombie *zomb, entity *attacker)
{
    if(zomb->zmb != NULL && zomb->zmb->entity_timer <= 0) {
        // damage flash

        play_override_animation(&zomb->zmb->animator_component, ANIM_ZOMBIE_DAMAGE);
        zomb->zmb->entity_timer = 0.35f;

        // knockback based on attacker position

        zomb->zmb->velocity[0] = attacker->position[0] >= zomb->zmb->position[0] ? -100.0f : 100.0f;
        zomb->zmb->velocity[1] = -200.0f;

        //zomb->zmb->collision_enabled = false;
        camera_shake(2.0f);
        zomb->zmb->health_points -= attacker->damage;
        zomb->zmb->gravity = ZOMBIE_GRAV;

        play_sound("enemy_hit.wav");

        // if zombie hp reaches 0

        if(zomb->zmb->health_points <= 0) 
        {
            kill_zombie(zomb);
            return;
        }
    }
}

void reset_zombie_progress() {
    time_between_each_zombie = STARTING_ZOMBIE_SPAWN_TIME;
    time_til_next_zombie = 0;

    power_timer = 0.0f;

    for(int i = 0; i < MAX_ZOMBIES; i++) {
        if(zombie_pool[i].enabled) {
            kill_zombie(&zombie_pool[i]);
        }
    }

    zombies_killed_total = 0;
    zombies_killed = 0;

    current_zombies = 0;
    current_max_zombies = STARTING_MAX_ZOMBIES;
}

// spawn zombie function, make new zombie out of object pool with passable info

void spawn_zombie(int tier, int hit_points, float speed, float jump_height)
{
    if(current_zombies < current_max_zombies) {
        // linear search through pool to find zombie to spawn, avoids dynamic memory allocation which is probably out of scope for a project this size

        for(int i = 0; i < MAX_ZOMBIES; i++)
        {
            // spawn first inactive zombie in pool, init all variables and such

            if(!zombie_pool[i].enabled)
            {
                zombie_pool[i].zmb = make_entity_in_scene(loaded_scene);
                play_animation(&zombie_pool[i].zmb->animator_component, &ANIM_MINION_IDLE);

                current_zombies += 1;

                // spawn in sky at any horizontal position in game-space. will fall down

                zombie_pool[i].zmb->position[0] = rand() % VIRTUAL_WIDTH;
                zombie_pool[i].zmb->position[1] = -64;

                // spawn at half gravity so when falling down the player has a chance to react

                zombie_pool[i].zmb->gravity = ZOMBIE_GRAV / 2;

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

                zombie_pool[i].zmb->entity_timer = 0;

                zombie_pool[i].enabled = true;

                // init zombies logic time a little off just so that all zombies dont behave in unison like clockwork

                zombie_pool[i].logic_frame_time = rand() % (1000 - 0 + 1) + 1;

                // convert from ms to seconds
                zombie_pool[i].logic_frame_time /= 1000;

                switch(tier)
                {
                    case 1:
                        zombie_pool[i].zmb->damage = 1;
                        break;
                    case 2:
                        zombie_pool[i].zmb->damage = 0;
                        break;
                    case 3:
                        zombie_pool[i].zmb->damage = 2;
                        break;
                }

                return;
            }
        }
    }
}

// this pathfinding is VERY primitive and relies on the level being mostly symmetrical with only a tiny bit of verticality. however it does work well enough for what were trying

void pathfinding_ai(zombie *zomb, entity *plyr)
{
    zomb->logic_frame_time += global_delta_time * loaded_scene->scene_game_speed;
    zomb->attack_time -= global_delta_time * loaded_scene->scene_game_speed;

    if(zomb->zmb->health_points >= 1) {
        if(zomb->zmb->entity_timer <= 0) {
            if(zomb->logic_frame_time >= 1.0f / ZOMBIE_LOGIC_TICK_RATE) 
            {
                zomb->logic_frame_time = 0.0f;

                // if player is on level with enemy. add some accounting for potential collision stuff by adding a little to the y value. use tile coordinates to be more exact.
                if((int)((plyr->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION) == (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION))
                {
                    // tell future ai logic what to do according with what its positional goal is/should be

                    zomb->pos_state_relative_to_player = ON_LEVEL_WITH_PLAYER;

                    // jump over gaps

                    if(loaded_scene->tilemap[(int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION)][(int)((zomb->zmb->position[0] + 0.1f) / LEVELS_TILE_RESOLUTION)].direction_for_pathing == 2
                        && zomb->zmb->is_grounded)
                    {
                        entity_override_velocity(zomb->zmb, (vec2){zomb->zmb->velocity[0] * 1.4f, -zomb->jump_height});
                    }
                }
                // if player is above enemy
                else if((int)((plyr->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION) < (int)((zomb->zmb->position[1] + 0.1f) / LEVELS_TILE_RESOLUTION))
                {
                    zomb->pos_state_relative_to_player = BELOW_PLAYER;

                    int jump_tiles_near_me[2] = {-1, -1};
                    bool has_found_tile = false;

                    // jump_tiles_near_me stores jumping tile coordinates in the row this zombie is in. find the nearest point where we can gain altitude

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
                // player is below, zomb is above
                else
                {
                    zomb->pos_state_relative_to_player = ABOVE_PLAYER;

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
                        // decide which is closer to begin moving to

                        int dist_from_tile_1 = glm_vec2_distance2((vec2){drop_tiles_near_me[0] * LEVELS_TILE_RESOLUTION, zomb->zmb->position[1]}, zomb->zmb->position);
                        int dist_from_tile_2 = glm_vec2_distance2((vec2){drop_tiles_near_me[1] * LEVELS_TILE_RESOLUTION, zomb->zmb->position[1]}, zomb->zmb->position);

                        if(dist_from_tile_1 >= dist_from_tile_2) zomb->zmb->velocity[0] = -zomb->speed;
                        else zomb->zmb->velocity[0] = zomb->speed;
                    }
                }
            }
        }

        // damage
            
        for(int i = 0; i < MAX_COLLIDING_ENTITIES; i++)
        {
            if(zomb->zmb != NULL && zomb->zmb->colliding_entities[i] != NULL && zomb->zmb->colliding_entities[i]->damage >= 1 
                && zomb->zmb->colliding_entities[i]->team != zomb->zmb->team && zomb->zmb->health_points >= 1) 
            {
                damage_zombie(zomb, zomb->zmb->colliding_entities[i]);
            }
        }
    }
}

void minion_ai(zombie *zomb, entity *plyr)
{
    // move toward player if on level

    if(zomb->pos_state_relative_to_player == ON_LEVEL_WITH_PLAYER)
    {
        zomb->zmb->velocity[0] = plyr->position[0] > zomb->zmb->position[0] ? zomb->speed : -zomb->speed;
    }

    // this zombie will never be idle so we can just do animations based on if were grounded or not

    if(zomb->zmb->is_grounded) play_animation(&zomb->zmb->animator_component, &ANIM_MINION_RUN);
    else play_animation(&zomb->zmb->animator_component, &ANIM_MINION_JUMP);

    // flip based on movement direction

    if(zomb->zmb->velocity[0] >= 0) zomb->zmb->sprite_data.flip_x = false;
    else zomb->zmb->sprite_data.flip_x = true;
}

void ranger_ai(zombie *zomb, entity *plyr)
{
    // use regular pathfinding, dont try to move upon reaching level with the player, snipe from a distance

    if(zomb->pos_state_relative_to_player == ON_LEVEL_WITH_PLAYER) zomb->zmb->velocity[0] = 0;

    if(zomb->attack_time <= 0)
    {
        make_bullet(&ENEMY_BULLETS, zomb->zmb->position, plyr->position[0] >= zomb->zmb->position[0] ? 1 : -1, 0);

        zomb->attack_time = RANGER_ATTACK_SPEED;
    }

    // flip based on player direction

    zomb->zmb->sprite_data.flip_x = plyr->position[0] > zomb->zmb->position[0] ? false : true;

    // ranger can be idle so animation state machine needs an extra if

    if(zomb->zmb->is_grounded)
    {
        if(zomb->zmb->velocity[0] == 0) play_animation(&zomb->zmb->animator_component, &ANIM_RANGER_IDLE);
        else play_animation(&zomb->zmb->animator_component, &ANIM_RANGER_RUN);
    }
    else play_animation(&zomb->zmb->animator_component, &ANIM_RANGER_JUMP);
}

// same as minion mostly

void king_ai(zombie *zomb, entity *plyr)
{
    // move toward player if on level

    if(zomb->pos_state_relative_to_player == ON_LEVEL_WITH_PLAYER)
    {
        zomb->zmb->velocity[0] = plyr->position[0] > zomb->zmb->position[0] ? zomb->speed : -zomb->speed;
    }

    // is basically always jumping up and down

    play_animation(&zomb->zmb->animator_component, &ANIM_KING_JUMP);

    // jump on being grounded

    if(zomb->zmb->is_grounded) {
        entity_override_velocity(zomb->zmb, (vec2){zomb->zmb->velocity[0], -zomb->jump_height});
        play_sound("king_jump.wav");
        camera_shake(1.0f);
    }

    // flip based on movement direction

    if(zomb->zmb->velocity[0] >= 0) zomb->zmb->sprite_data.flip_x = false;
    else zomb->zmb->sprite_data.flip_x = true;
}

void zombie_spawner_and_tracker()
{
    time_til_next_zombie += global_delta_time * loaded_scene->scene_game_speed;
    power_timer += global_delta_time * loaded_scene->scene_game_speed;

    // avoid crashes

    if(STARTING_MAX_ZOMBIES + (ADD_ZOMBIE_LIMIT_PER_MINUTE * (floor(power_timer / 60))) < MAX_ZOMBIES) 
        current_max_zombies = STARTING_MAX_ZOMBIES + (ADD_ZOMBIE_LIMIT_PER_MINUTE * (floor(power_timer / 60)));

    // increase hp based on how much t ime has elapsed

    minion_hp = MINION_BASE_HP + (floor(power_timer / MINION_HP_GAIN_TIME));
    ranger_hp = RANGER_BASE_HP + (floor(power_timer / RANGER_HP_GAIN_TIME));
    king_hp = KING_BASE_HP + (floor(power_timer / KING_HP_GAIN_TIME));
    
    // if its time to spawn a zombie, random chance to skip

    if(time_til_next_zombie >= time_between_each_zombie)
    {
        time_between_each_zombie /= SPAWN_TIME_MULT_RATE;
        time_til_next_zombie = 0;

        for(int i = 0; i < rand() % 2; i++)
        {
            int zom_to_spawn = rand() % 10;

            // 0 indexed obviously

            int hp_for_tier[4] = { 0, minion_hp, ranger_hp, king_hp };

            int tier = 1;
            if (zom_to_spawn > 7 && power_timer >= TIME_TIL_KING) tier = 3;
            else if (zom_to_spawn > 5 && power_timer >= TIME_TIL_RANGER) tier = 2;

            spawn_zombie(tier, hp_for_tier[tier], DEAFULT_ZOMBIE_SPEED, DEAFULT_ZOMBIE_JUMP_HEIGHT);
        }
    }
}

void simulate_zombies(entity *player)
{
    // iterate through all zombies and perform logic

    for(int i = 0; i < MAX_ZOMBIES; i++)
    {
        if(zombie_pool[i].enabled && zombie_pool[i].zmb != NULL) 
        {
            // zombies fall down slower upon spawning to be fairer, upon reaching ground they return to normal
            
            if(zombie_pool[i].zmb->is_grounded) zombie_pool[i].zmb->gravity = ZOMBIE_GRAV;

            // only perform if not stunned (i.e entity timer 0 or below)

            if(zombie_pool[i].zmb->entity_timer <= 0) {
                switch (zombie_pool[i].tier)
                {
                    case 1:
                        minion_ai(&zombie_pool[i], player);
                        break;
                    
                    case 2:
                        ranger_ai(&zombie_pool[i], player);
                        break;

                    case 3:
                        king_ai(&zombie_pool[i], player);
                        break;
                }
            }

            pathfinding_ai(&zombie_pool[i], player);
        }
    }
}

void zombie_coreloop(entity* plyr)
{
    zombie_spawner_and_tracker();
    simulate_zombies(plyr);
}