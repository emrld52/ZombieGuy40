#include "r_renderfuncs.h"
#include "z_coreloop.h"

// cglm gl math

#include "../deps/cglm/cglm.h"

sprite player;
sprite player2;

void gameloop_init()
{
    glm_vec3_copy( (vec3){ 0.0f, 0.0f, 0.0f }, player.pos);
    glm_vec3_copy( (vec3){ 32.0f, 32.0f, 0.0f }, player2.pos);
    player.sprite_coord[0] = 2;
    player.sprite_coord[1] = 1;
    player2.sprite_coord[0] = 2;
    player2.sprite_coord[1] = 1;
}

void run_gameloop()
{
    draw_call(player);
    draw_call(player2);

    player2.pos[0] += 0.1f;
    player.pos[1] += 0.1f;
}