#include <stdio.h>
#include <stdlib.h>

#define SOKOL_IMPL
#define SOKOL_D3D11

#include "../deps/sokol_app.h"

void init()
{

}


void frame()
{

}

void cleanup()
{
    
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
        .window_title = "fart"
    };
}