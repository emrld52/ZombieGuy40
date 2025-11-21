#pragma once

// game logic

void gameloop_init();
void run_gameloop();

// program life cycle stuff

void program_init();
void program_loop();
void program_cleanup();

void load_scene(int scn);

#define MAX_LOADED_SCENES 4