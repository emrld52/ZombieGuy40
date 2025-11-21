// THIS CODE ACHIEVES VERY SIMPLE TEXT RENDERING USING SPRITES AND SIMPLE STRING PARSING

#include "s_text_renderer.h"

// for vec2 for passing in position to text renderer

#include "../deps/cglm/cglm.h"

// fixed amount of numbers but theoretically if you wanted to add more letters for different language support you can

sprite LETTER_SPRITES[LETTER_COUNT];
sprite NUMBER_SPRITES[10];

void init_text_renderer()
{
    for(int i = 0; i < LETTER_COUNT; i++)
    {
        LETTER_SPRITES[i].resolution[0] = 16;
        LETTER_SPRITES[i].resolution[1] = 16;

        // font sheet is 9x4

        LETTER_SPRITES[i].sprite_coord[0] = FONT_SHEET_X + (i % FONT_SHEET_WIDTH);
        LETTER_SPRITES[i].sprite_coord[1] = FONT_SHEET_Y + ceil(i / FONT_SHEET_WIDTH);;

        LETTER_SPRITES[i].ui = true;
    }

    // 0 is on row with letters

    NUMBER_SPRITES[0].resolution[0] = 16;
    NUMBER_SPRITES[0].resolution[1] = 16;

    NUMBER_SPRITES[0].sprite_coord[0] = FONT_SHEET_X + FONT_SHEET_WIDTH - 1;
    NUMBER_SPRITES[0].sprite_coord[1] = FONT_SHEET_Y + FONT_SHEET_HEIGHT - 2;

    NUMBER_SPRITES[0].ui = true;

    for(int i = 1; i < 10; i++)
    {
        NUMBER_SPRITES[i].resolution[0] = 16;
        NUMBER_SPRITES[i].resolution[1] = 16;

        NUMBER_SPRITES[i].sprite_coord[0] = FONT_SHEET_X + i - 1;
        NUMBER_SPRITES[i].sprite_coord[1] = FONT_SHEET_Y + FONT_SHEET_HEIGHT - 1;

        NUMBER_SPRITES[i].ui = true;
    }
}

int how_wide_is_text(int len)
{
    return (len * LETTER_DISTANCE) + 16;
}

bool is_point_within_text(vec2 pos, int len, vec2 point)
{
    return point[0] <= pos[0] + how_wide_is_text(len) && point[0] >= pos[0] && point[1] <= pos[1] + 16 && point[1] >= pos[1];
}

// render by iterating through all text, finding the matching letter and applying the position

void render_text(char txt[], int len, vec2 pos)
{
    int offset_text = 0;
    int offset_y = 0;
    int last_line_break_pos = 0;

    for(int i = 0; i < len; i++)
    {
        // not cleanest way to do this but it works. sets position of text

        int pos_x; 
        if(offset_text == 0) pos_x = pos[0] + ((i - last_line_break_pos) * LETTER_DISTANCE);
        else pos_x = pos[0] + ((i - last_line_break_pos - 2) * LETTER_DISTANCE);
        int pos_y = pos[1] + (offset_y * 16);

        // line break functionality
        if(i < len - 1 && txt[i] == '/' && txt[i + 1] == 'n')
        {
            last_line_break_pos = i;
            i += 1;
            offset_text += 1;
            offset_y += 1;
            continue;
        }

        // from here pick the letter to display

        switch(txt[i])
        {
            case 'a':
                LETTER_SPRITES[0].pos[0] = pos_x;
                LETTER_SPRITES[0].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[0]);
                break;
            case 'b':
                LETTER_SPRITES[1].pos[0] = pos_x;
                LETTER_SPRITES[1].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[1]);
                break;
            case 'c':
                LETTER_SPRITES[2].pos[0] = pos_x;
                LETTER_SPRITES[2].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[2]);
                break;
            case 'd':
                LETTER_SPRITES[3].pos[0] = pos_x;
                LETTER_SPRITES[3].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[3]);
                break;
            case 'e':
                LETTER_SPRITES[4].pos[0] = pos_x;
                LETTER_SPRITES[4].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[4]);
                break;
            case 'f':
                LETTER_SPRITES[5].pos[0] = pos_x;
                LETTER_SPRITES[5].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[5]);
                break;
            case 'g':
                LETTER_SPRITES[6].pos[0] = pos_x;
                LETTER_SPRITES[6].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[6]);
                break;
            case 'h':
                LETTER_SPRITES[7].pos[0] = pos_x;
                LETTER_SPRITES[7].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[7]);
                break;
            case 'i':
                LETTER_SPRITES[8].pos[0] = pos_x;
                LETTER_SPRITES[8].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[8]);
                break;
            case 'j':
                LETTER_SPRITES[9].pos[0] = pos_x;
                LETTER_SPRITES[9].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[9]);
                break;
            case 'k':
                LETTER_SPRITES[10].pos[0] = pos_x;
                LETTER_SPRITES[10].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[10]);
                break;
            case 'l':
                LETTER_SPRITES[11].pos[0] = pos_x;
                LETTER_SPRITES[11].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[11]);
                break;
            case 'm':
                LETTER_SPRITES[12].pos[0] = pos_x;
                LETTER_SPRITES[12].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[12]);
                break;
            case 'n':
                LETTER_SPRITES[13].pos[0] = pos_x;
                LETTER_SPRITES[13].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[13]);
                break;
            case 'o':
                LETTER_SPRITES[14].pos[0] = pos_x;
                LETTER_SPRITES[14].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[14]);
                break;
            case 'p':
                LETTER_SPRITES[15].pos[0] = pos_x;
                LETTER_SPRITES[15].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[15]);
                break;
            case 'q':
                LETTER_SPRITES[16].pos[0] = pos_x;
                LETTER_SPRITES[16].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[16]);
                break;
            case 'r':
                LETTER_SPRITES[17].pos[0] = pos_x;
                LETTER_SPRITES[17].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[17]);
                break;
            case 's':
                LETTER_SPRITES[18].pos[0] = pos_x;
                LETTER_SPRITES[18].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[18]);
                break;
            case 't':
                LETTER_SPRITES[19].pos[0] = pos_x;
                LETTER_SPRITES[19].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[19]);
                break;
            case 'u':
                LETTER_SPRITES[20].pos[0] = pos_x;
                LETTER_SPRITES[20].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[20]);
                break;
            case 'v':
                LETTER_SPRITES[21].pos[0] = pos_x;
                LETTER_SPRITES[21].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[21]);
                break;
            case 'w':
                LETTER_SPRITES[22].pos[0] = pos_x;
                LETTER_SPRITES[22].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[22]);
                break;
            case 'x':
                LETTER_SPRITES[23].pos[0] = pos_x;
                LETTER_SPRITES[23].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[23]);
                break;
            case 'y':
                LETTER_SPRITES[24].pos[0] = pos_x;
                LETTER_SPRITES[24].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[24]);
                break;
            case 'z':
                LETTER_SPRITES[25].pos[0] = pos_x;
                LETTER_SPRITES[25].pos[1] = pos_y;

                draw_call(LETTER_SPRITES[25]);
                break;
            case '0':
                NUMBER_SPRITES[0].pos[0] = pos_x;
                NUMBER_SPRITES[0].pos[1] = pos_y;

                draw_call(NUMBER_SPRITES[0]);
                break;
            case '1':
                NUMBER_SPRITES[1].pos[0] = pos_x;
                NUMBER_SPRITES[1].pos[1] = pos_y;

                draw_call(NUMBER_SPRITES[1]);
                break;
            case '2':
                NUMBER_SPRITES[2].pos[0] = pos_x;
                NUMBER_SPRITES[2].pos[1] = pos_y;

                draw_call(NUMBER_SPRITES[2]);
                break;
            case '3':
                NUMBER_SPRITES[3].pos[0] = pos_x;
                NUMBER_SPRITES[3].pos[1] = pos_y;

                draw_call(NUMBER_SPRITES[3]);
                break;
            case '4':
                NUMBER_SPRITES[4].pos[0] = pos_x;
                NUMBER_SPRITES[4].pos[1] = pos_y;

                draw_call(NUMBER_SPRITES[4]);
                break;
            case '5':
                NUMBER_SPRITES[5].pos[0] = pos_x;
                NUMBER_SPRITES[5].pos[1] = pos_y;

                draw_call(NUMBER_SPRITES[5]);
                break;
            case '6':
                NUMBER_SPRITES[6].pos[0] = pos_x;
                NUMBER_SPRITES[6].pos[1] = pos_y;

                draw_call(NUMBER_SPRITES[6]);
                break;
            case '7':
                NUMBER_SPRITES[7].pos[0] = pos_x;
                NUMBER_SPRITES[7].pos[1] = pos_y;

                draw_call(NUMBER_SPRITES[7]);
                break;
            case '8':
                NUMBER_SPRITES[8].pos[0] = pos_x;
                NUMBER_SPRITES[8].pos[1] = pos_y;

                draw_call(NUMBER_SPRITES[8]);
                break;
            case '9':
                NUMBER_SPRITES[9].pos[0] = pos_x;
                NUMBER_SPRITES[9].pos[1] = pos_y;

                draw_call(NUMBER_SPRITES[9]);
                break;
        }
    }
}