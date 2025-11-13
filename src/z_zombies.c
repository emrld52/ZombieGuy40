#include "z_zombies.h"

// libs

#include "../deps/cglm/cglm.h"
#include <stdbool.h>
#include <stdlib.h>

// game logic

#include "g_state.h"
#include "z_coreloop.h"
#include "z_player.h"

// rendering

#include "r_renderfuncs.h"

#define ZOMBIE_GRAV 1024.0f

// object pooling

zombie zombie_pool[MAX_ZOMBIES];

// spawn zombie function, make new zombie out of object pool with passable info

void spawn_zombie(int tier, int hit_points, float speed, float jump_height)
{
    // linear search through pool to find zombie to spawn, avoids dynamic memory allocation which is probably out of scope for a project this size

    for(int i = 0; i < MAX_ZOMBIES; i++)
    {
        // spawn first inactive zombie in pool

        if(!zombie_pool[i].enabled)
        {
            // init zombie sprite, no animations yet

            zombie_pool[i].zombie_sprite.sprite_coord[0] = 1;
            zombie_pool[i].zombie_sprite.sprite_coord[1] = 5 + ((rand() % 3) * 3);
            zombie_pool[i].zombie_sprite.resolution[0] = 32;
            zombie_pool[i].zombie_sprite.resolution[1] = 32;
            zombie_pool[i].zombie_sprite.ui = false;

            // spawn in sky at any horizontal position in game-space. will fall down

            zombie_pool[i].zombie_sprite.pos[0] = rand() % (render_game_width - 0 + 1) + 1;
            zombie_pool[i].zombie_sprite.pos[1] = -64;

            zombie_pool[i].tier = tier;

            zombie_pool[i].hit_points = hit_points;
            zombie_pool[i].speed = speed;
            zombie_pool[i].jump_height = jump_height;

            zombie_pool[i].time_til_next_jump_impulse = 0;

            zombie_pool[i].enabled = true;

            zombie_pool[i].logic_frame_time = rand() % (1000 - 0 + 1) + 1;;

            // convert from ms to seconds
            zombie_pool[i].logic_frame_time /= 1000;

            return;
        }
    }
}

void simulate_zombies(sprite player)
{
    for(int i = 0; i < MAX_ZOMBIES; i++)
    {
        if(zombie_pool[i].enabled == true)
        {
            // frame time counter for certain logic

            zombie_pool[i].logic_frame_time += global_delta_time * global_game_speed;

            // gravity

            if(zombie_pool[i].zombie_sprite.pos[1] + 32 < 448) 
            {
                zombie_pool[i].is_grounded = false;
                zombie_pool[i].vel[1] += ZOMBIE_GRAV * global_delta_time * global_game_speed;
            }
            else 
            {
                if(zombie_pool[i].zombie_sprite.pos[1] + 32 > 448) zombie_pool[i].zombie_sprite.pos[1] = 448 - 32;

                zombie_pool[i].is_grounded = true;
                zombie_pool[i].vel[1] = 0;
            }

            if(player.pos[0] >= zombie_pool[i].zombie_sprite.pos[0]) zombie_pool[i].vel[0] = zombie_pool[i].speed;
            else zombie_pool[i].vel[0] = -zombie_pool[i].speed;

            // perform random jumps every few seconds to make zombies look slightly more alive

            zombie_pool[i].time_til_next_jump_impulse -= global_delta_time * global_game_speed;

            if(zombie_pool[i].is_grounded) 
            {
                // perform this check once every second and a half

                if(player.pos[1] < zombie_pool[i].zombie_sprite.pos[1] && zombie_pool[i].logic_frame_time >= 1.5f) 
                {
                    zombie_pool[i].vel[1] = -zombie_pool[i].jump_height;

                    // reset logic timer
                    zombie_pool[i].logic_frame_time = 0.0f;
                }

                if(zombie_pool[i].time_til_next_jump_impulse <= 0.0f)
                {
                    // small impulse jumps shouldnt be as big as actual jumps

                    zombie_pool[i].vel[1] = -zombie_pool[i].jump_height / 3;
                    zombie_pool[i].time_til_next_jump_impulse = rand() % (6 - 1) + 1;
                }
            }
            

            // add velocity to position. first index (0) = x axis, second index = y

            zombie_pool[i].zombie_sprite.pos[0] += zombie_pool[i].vel[0] * global_delta_time * global_game_speed;
            zombie_pool[i].zombie_sprite.pos[1] += zombie_pool[i].vel[1] * global_delta_time * global_game_speed;

            //printf("\nzom %d pos %f %f logic timer %f time til next jump %f\n", i, zombie_pool[i].zombie_sprite.pos[0], zombie_pool[i].zombie_sprite.pos[1], zombie_pool[i].logic_frame_time, zombie_pool[i].time_til_next_jump_impulse);
        }
    }
}

void render_zombies()
{
    for(int i = 0; i < MAX_ZOMBIES; i++)
    {
        if(zombie_pool[i].enabled)
        {
            draw_call(zombie_pool[i].zombie_sprite);
        }
    }
}