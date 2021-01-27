#include <iostream>
#include <string>

#include <board_generator.h>
#include <game.h>


#include "colors.cpp"

using std::vector;

void Board_Generator::draw_board() {
    my_display::initiate_colors();
    clear();
    for(int row = 0; row < _length; row++) {
        my_display::draw_vertical_border(row, 0);
        for(int col = 1; col <= _width-2; col++) {
            if(row == 0 || row == _length-1) {
                my_display::draw_horizontal_border(row, col);
            } else {     
                my_display::draw_empty_cell(row, col);
            }
        }  
        my_display::draw_vertical_border(row, _width - 1);
        printw("\n");
    }
    printw(" ");
    int r,c;
    getyx(stdscr, r, c);
    my_display::draw_player(r, c);
    printw(" = Player\n\n ");
    getyx(stdscr, r, c);
    my_display::draw_obstacle(r, c);
    printw(" = Incoming blocks\n");
    printw("Use ARROW KEYs to avoid block and up your score\n");
    refresh();
}

void Board_Generator::update_cell(int row, int col, int val, int prev_row, int prev_col) {
    std::lock_guard<std::mutex> locker(_mutex);
    if(val == 2) {
        my_display::draw_player(row + 1, col + 1);
        my_display::draw_empty_cell(prev_row + 1, prev_col + 1);
        refresh();
    }
}

void Board_Generator::update_cell(int row, int col, int val) {
    std::lock_guard<std::mutex> locker(_mutex);
    if(val == 2) {
        my_display::draw_player(row + 1, col + 1);
        refresh();
    }
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
    for(int c = 0; c < cols.size(); c++) {
        if(val == 1) {
            my_display::draw_obstacle(row + 1, cols[c] + 1);
        } else {
            my_display::draw_empty_cell(row + 1, cols[c] + 1);
        }
    }
    refresh();
}

void Board_Generator::update_cells(int row, int val) {
    std::lock_guard<std::mutex> locker(_mutex);
    for(int col = 1; col <= _width-2; col++) {
        if(val == 0) {
            my_display::draw_empty_cell(row + 1, col);
        } else {
            my_display::draw_obstacle(row + 1, col);
        }
    }
    refresh();
}

void Board_Generator::empty_the_cell(int row, int col) {
    std::lock_guard<std::mutex> locker(_mutex);
    my_display::draw_empty_cell(row + 1, col + 1);
    refresh();
}
