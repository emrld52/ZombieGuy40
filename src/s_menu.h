#pragma once

#include "r_renderfuncs.h"
#include "s_animation.h"

void init_main_menu();
void draw_main_menu();

#define MAX_BUTTON_TEXT_LEN 10

typedef struct button_t
{
    animator button_animation;
    sprite button_strip;
    char txt[MAX_BUTTON_TEXT_LEN];
    bool was_hovered;
} button;