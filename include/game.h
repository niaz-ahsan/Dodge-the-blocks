#ifndef GAME_H
#define GAME_H

#include <vector>
#include <curses.h>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <future>

#include "board_generator.h"

class Game {
public: 
    Game(int r, int c, std::unique_ptr<Board_Generator> board) : _row(r-2), _col(c-2), _board(std::move(board)) {
        // determie initial gap in each obstacle
        obstacle_gap = (gap_percentage_compared_to_width * _col) / 100;
        obstacle_stream_delay = 2000;
        obstacle_moving_delay = 400;
    }
    ~Game() {} //delwin(_win); }
    void load_game();
    void launch_game();
    void print_inner_board(); // dummy method for test
private:
    std::vector<std::vector<int> > _inner_board;
    int _row;
    int _col;
    int score = 0;
    int gap_percentage_compared_to_width = 10; // 10% gap compared to the overall width of inner matrix
    //WINDOW *_win;
    int obstacle_stream_delay;  // inscreases to get more dense obstacle
    int obstacle_moving_delay; 
    int obstacle_gap; // gap for vehicle to pass through
    std::unique_ptr<Board_Generator> _board;
    std::mutex _mutex;
    std::condition_variable _cv;
    bool game_should_go_on = true; 
    bool vehicle_created = false; // obstacle generation should wait until player/vehicle is created... this var controls that
    int gap_generation_direction = 1;
    void move_my_vehicle();
    void generate_obstacles();
    void generate_single_obstacle(int, int);
    void change_inner_board_value(int, int, int);// change a single value
    void change_inner_board_value(int, int); // change values of a whole row
    void change_inner_board_value(int, std::vector<int>&, int); // change values of a row specified by cols
    int get_inner_board_cell(int, int);
    void post_game_over();
    bool check_collision_from_vehicle(int, int);
    bool check_collision_from_obstacle(int, std::vector<int>&);
    void stop_game();
    int get_gap_start_index(int);
    void update_obstacle_delays();
    //int get_obstacle_delay();
};

#endif