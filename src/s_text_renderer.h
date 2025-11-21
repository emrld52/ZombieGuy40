#include "r_renderfuncs.h"

#define LETTER_COUNT 26

#define FONT_SHEET_WIDTH 9
#define FONT_SHEET_HEIGHT 4
#define FONT_SHEET_X 12
#define FONT_SHEET_Y 5

#define LETTER_DISTANCE 15

// sprites for each letter

extern sprite LETTER_SPRITES[LETTER_COUNT];
extern sprite NUMBER_SPRITES[10];

void init_text_renderer();
int how_wide_is_text(int len);

// assuming text uses no line breaks. used mainly for mouse and such

bool is_point_within_text(vec2 pos, int len, vec2 point);

void render_text(char txt[], int len, vec2 pos);