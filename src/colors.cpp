#ifndef COLORS
#define COLORS

#include <curses.h>

#define GRASS_PAIR     1
#define WATER_PAIR     2
#define MOUNTAIN_PAIR  3
#define PLAYER_PAIR    4

namespace my_display {
    inline void initiate_colors() {
        start_color();
        init_pair(GRASS_PAIR, COLOR_YELLOW, COLOR_GREEN);
        init_pair(WATER_PAIR, COLOR_CYAN, COLOR_BLUE);
        init_pair(MOUNTAIN_PAIR, COLOR_BLACK, COLOR_WHITE);
        init_pair(PLAYER_PAIR, COLOR_RED, COLOR_GREEN);
    }

    inline void draw_horizontal_border(int row, int col) {
        mvaddch(row, col, ACS_HLINE);
    } 

    inline void draw_vertical_border(int row, int col) {
        mvaddch(row, col, ACS_VLINE);
    }

    inline void draw_empty_cell(int row, int col) {
        attron(COLOR_PAIR(WATER_PAIR));
        mvaddch(row, col, ACS_BULLET);
        attroff(COLOR_PAIR(WATER_PAIR));
    }

    inline void draw_player(int row, int col) {
        attron(COLOR_PAIR(PLAYER_PAIR));
        mvaddch(row, col, ACS_DIAMOND);
        attroff(COLOR_PAIR(PLAYER_PAIR));
    }

    inline void draw_obstacle(int row, int col) {
        attron(COLOR_PAIR(MOUNTAIN_PAIR));
        mvaddch(row, col, ACS_CKBOARD);
        attroff(COLOR_PAIR(MOUNTAIN_PAIR));
    }
}

#endif