#include <stdio.h>
#include <stdlib.h>

// sokol stuff

#define SOKOL_IMPL
#define SOKOL_D3D11

#include "../deps/sokol_app.h"
#include "../deps/sokol_gfx.h"
#include "../deps/sokol_glue.h"
#include "../deps/sokol_log.h"
#include "../deps/sokol_time.h"

// use stb image for image decoding

#define STB_IMAGE_IMPLEMENTATION

#include "../deps/stb_image.h"

// shaders

#include "../shaders/shaders.glsl.h"

// cglm gl math

#include "../deps/cglm/cglm.h"

// include src files

#include "r_renderfuncs.h"
#include "z_coreloop.h"

// player

#include "z_player.h"

// global state

#include "g_state.h"

// init input

input_reader global_input = { 0 };
float global_game_speed = 1.0f;

static void init(void)
{ 
    memset(&global_input, 0, sizeof(input_reader));

    gameloop_init();
    init_rendering();
}


void frame()
{
    run_gameloop();
    draw_game();
}

void cleanup()
{
    sg_shutdown();
}

// poll input

void event(const sapp_event* ev) {
    switch (ev->type) {
        case SAPP_EVENTTYPE_KEY_DOWN:
            if (ev->key_code < 512) {
                global_input.keysPressed[ev->key_code] = true;
            }
            break;

        case SAPP_EVENTTYPE_KEY_UP:
            if (ev->key_code < 512) {
                global_input.keysPressed[ev->key_code] = false;
            }
            break;

        default:
            
            break;
    }
}
            
sapp_desc sokol_main(int argc, char* argv[]) {
    return (sapp_desc) {
        .width = 640,
        .height = 480,
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .window_title = "ZombieGuy40"
    };
}