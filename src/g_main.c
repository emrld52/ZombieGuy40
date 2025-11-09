#include <stdio.h>
#include <stdlib.h>

// sokol stuff

#define SOKOL_IMPL
#define SOKOL_GLCORE

#include "../deps/sokol_app.h"
#include "../deps/sokol_gfx.h"
#include "../deps/sokol_glue.h"

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

static void init(void)
{
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

void event()
{

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