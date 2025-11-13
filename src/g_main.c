#include <stdio.h>
#include <stdlib.h>

// sokol stuff

#define SOKOL_IMPL
#define SOKOL_D3D11

// libs

#include "../deps/sokol_app.h"
#include "../deps/sokol_gfx.h"
#include "../deps/sokol_glue.h"
#include "../deps/sokol_log.h"
#include "../deps/sokol_time.h"
#include "../deps/cglm/cglm.h"

#define STB_IMAGE_IMPLEMENTATION

#include "../deps/stb_image.h"

// handles program life cycle

#include "z_coreloop.h"

// program entry point called by sokol app header. all callback functions found in coreloop file
            
sapp_desc sokol_main(int argc, char* argv[]) {
    return (sapp_desc) {
        .width = 640,
        .height = 480,
        .init_cb = program_init,
        .frame_cb = program_loop,
        .cleanup_cb = program_cleanup,
        .event_cb = program_event,
        .window_title = "ZombieGuy40"
    };
}