#define MINIAUDIO_IMPLEMENTATION

#include "../deps/miniaudio.h"
#include "s_sound.h"

ma_result result;
ma_engine engine;

void init_audio() {
    result = ma_engine_init(NULL, &engine);
    if(result != MA_SUCCESS) return;
}

// pass in using enum names in header

void play_sound(char file[]) {
    ma_engine_play_sound(&engine, file, NULL);
}

void uninit_sound() {
    ma_engine_uninit(&engine);
}