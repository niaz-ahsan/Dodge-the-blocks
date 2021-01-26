#ifndef COLORS
#define COLORS

#include <curses.h>

#define GRASS_PAIR     1
#define WATER_PAIR     2
#define MOUNTAIN_PAIR  3
#define PLAYER_PAIR    4

namespace my_display {
    void initiate_colors() {
        start_color();
        init_pair(GRASS_PAIR, COLOR_YELLOW, COLOR_GREEN);
        init_pair(WATER_PAIR, COLOR_CYAN, COLOR_BLUE);
        init_pair(MOUNTAIN_PAIR, COLOR_BLACK, COLOR_WHITE);
        init_pair(PLAYER_PAIR, COLOR_RED, COLOR_GREEN);
    }
}

#endif