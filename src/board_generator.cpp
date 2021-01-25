#include <iostream>
#include <string>

#include <board_generator.h>
#include <game.h>

#define GRASS_PAIR     1
#define WATER_PAIR     2
#define MOUNTAIN_PAIR  3
#define PLAYER_PAIR    4

using std::vector;

void Board_Generator::draw_board() {
    initiate_colors();
    clear();
    for(int row = 0; row < _length; row++) {
        //mvwprintw(_win, row, 0, "|");
        draw_vertical_border(row, 0);
        for(int col = 1; col <= _width-2; col++) {
            if(row == 0 || row == _length-1) {
                //mvwprintw(_win, row, col, "=");
                draw_horizontal_border(row, col);
            } else {     
                //mvwprintw(_win, row, col, " ");
                draw_empty_cell(row, col);
            }
        }  
        //mvwprintw(_win, row, _width-1, "|\n");
        draw_vertical_border(row, _width - 1);
        printw("\n");
    }
    printw("*  = Player\n");
    printw("-  = Incoming blocks\n");
    printw("Use ARROW KEYs to avoid block and up your score\n");
    wrefresh(_win);
}

void Board_Generator::initiate_colors() {
    start_color();
    init_pair(GRASS_PAIR, COLOR_YELLOW, COLOR_GREEN);
    init_pair(WATER_PAIR, COLOR_CYAN, COLOR_BLUE);
    init_pair(MOUNTAIN_PAIR, COLOR_BLACK, COLOR_WHITE);
    init_pair(PLAYER_PAIR, COLOR_RED, COLOR_MAGENTA);
}

void Board_Generator::update_cell(int row, int col, int val, int prev_row, int prev_col) {
    std::lock_guard<std::mutex> locker(_mutex);
    const char *new_val = (val == 2)? "*" : "|";
    mvwprintw(_win, row+1, col+1, new_val);
    mvwprintw(_win, prev_row+1, prev_col+1, " ");
    wrefresh(_win); 
}

void Board_Generator::update_cell(int row, int col, int val) {
    std::lock_guard<std::mutex> locker(_mutex);
    const char *new_val = (val == 2)? "*" : "|";
    mvwprintw(_win, row+1, col+1, new_val);
    wrefresh(_win); 
}

/*void Board_Generator::update_cells(int row, vector<int> &cols, int val) {
    std::lock_guard<std::mutex> locker(_mutex);
    for(int i=0; i<cols.size(); i++) {
        if(row > 0) { // preventing change when modifying 1st row
            mvwprintw(_win, row, cols[i]+1, " ");    
        }
        if(row+1 < _length-1) { // preventing modifying board border
            mvwprintw(_win, row+1, cols[i]+1, "-");
        }       
    }
    wrefresh(_win);
}*/

void Board_Generator::update_cells(int row, vector<int> &cols, int val) {
    std::lock_guard<std::mutex> locker(_mutex);
    const char *new_val = (val == 1)? "-" : " ";
    for(int c = 0; c < cols.size(); c++) {
        mvwprintw(_win, row + 1, cols[c] + 1, new_val);
    }
    wrefresh(_win);
}

void Board_Generator::update_cells(int row, int val) {
    std::lock_guard<std::mutex> locker(_mutex);
    const char *new_val = (val == 0)? " " : "-";
    for(int col = 1; col <= _width-2; col++) {
        mvwprintw(_win, row + 1, col, new_val);
    }
    wrefresh(_win);
}

void Board_Generator::empty_the_cell(int row, int col) {
    std::lock_guard<std::mutex> locker(_mutex);
    mvwprintw(_win, row + 1, col + 1, " ");
    wrefresh(_win);
}

void Board_Generator::draw_horizontal_border(int row, int col) {
    mvaddch(row, col, ACS_HLINE);
} 

void Board_Generator::draw_vertical_border(int row, int col) {
    mvaddch(row, col, ACS_VLINE);
}

void Board_Generator::draw_empty_cell(int row, int col) {
    attron(COLOR_PAIR(WATER_PAIR));
    mvaddch(row, col, ACS_BULLET);
    attroff(COLOR_PAIR(WATER_PAIR));
}