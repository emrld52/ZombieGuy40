#define MINIAUDIO_IMPLEMENTATION

#include "../deps/miniaudio.h"
#include "s_sound.h"

ma_result result;
ma_engine engine;

ma_sound song;

float song_vol;

void init_audio() {
    result = ma_engine_init(NULL, &engine);
    if(result != MA_SUCCESS) return;

    if (ma_sound_init_from_file(&engine, "zombie_killer.wav", 0, NULL, NULL, &song) != MA_SUCCESS) return;

    song_vol = 1.0f;
}

void play_song()
{
    ma_sound_start(&song);
    //ma_sound_set_volume(&song, song_vol);
}

// pass in using enum names in header

void play_sound(char file[]) {
    ma_engine_play_sound(&engine, file, NULL);
}

void uninit_sound() {
    ma_engine_uninit(&engine);
}