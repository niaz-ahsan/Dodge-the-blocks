#ifndef BOARD_GENERATOR_H
#define BOARD_GENERATOR_H

#include <vector>
#include <curses.h>
#include <mutex>
#include <memory>

using std::vector;

class Board_Generator {
public:
    Board_Generator(int &l, int &w, WINDOW *win) : _length(l), _width(w), _win(win) {} 
    ~Board_Generator() { delwin(_win); }

    void draw_board();
    void update_cell(int, int, int, int, int); // specially for updating player/vehicle state change & player bullet movement
    void update_cells(int, vector<int>&, int); // specially for blocks state change
    void update_cells(int, int); // specially for obstacles row wise
    void update_cell(int, int, int); // just to change one cell and not undoing any
    void empty_the_cell(int, int);
private:
    unsigned int _length;
    unsigned int _width; 
    WINDOW *_win;
    std::mutex _mutex;

    void initiate_colors();
    void draw_horizontal_border(int, int);
    void draw_vertical_border(int, int);
    void draw_player(int, int);
    void draw_obstacle(int, int);
    void draw_empty_cell(int, int);
};

#endif