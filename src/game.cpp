#include <chrono>

#include <game.h>
#include <curses.h>
#include <pthread.h>

/*
Board value:
0 - Empty cell
1 - Obstacle (-)
2 - Vehicle (*)
gap_generation_direction - 1 : generate on right of prev | 0 : generate on left of prev
*/

void Game::load_game() {
    // initialize inner vector
    std::vector<int> inner;
    for (int i = 0; i < _row; i++) {
        inner.clear();
        for (int j = 0; j < _col; j++) {
            inner.push_back(0);
        }
        _inner_board.push_back(inner);
    }
}

void Game::launch_game() {
    _board->draw_board();

    // generating and moving obstacle continuously
    std::future<void> obstacle_thread = std::async(std::launch::async, &Game::generate_obstacles, this);
    //std::thread obstacle_thread(&Game::generate_obstacles, this);

    // creating my vehicle thread
    std::future<void> vehicle_thread = std::async(std::launch::async, &Game::move_my_vehicle, this);
    //std::thread vehicle_thread(&Game::move_my_vehicle, this);
    
    // returns once game is over
    obstacle_thread.wait();
    vehicle_thread.wait();

    // display text after game is over
    post_game_over();
}

void Game::move_my_vehicle() {
    int vehicle_row = _row-1;
    int vehicle_col = _col/2;   
    change_inner_board_value(vehicle_row, vehicle_col, 2); 
    _board->update_cell(vehicle_row, vehicle_col, 2, -2, -2);
    
    // notifying obstacle generation thread to create obstacle
    if(!vehicle_created) {
        std::unique_lock<std::mutex> locker(_mutex);
        vehicle_created = true;
        _cv.notify_all();
    }

    int pressed_key;
    while(game_should_go_on) {
        pressed_key = getch();
        switch (pressed_key) {
            case KEY_UP:
                if(vehicle_row > 0) { 
                    change_inner_board_value(vehicle_row, vehicle_col, 0);
                    vehicle_row -= 1;
                    if(check_collision_from_vehicle(vehicle_row, vehicle_col)) {
                        stop_game();
                        break;
                    }
                    change_inner_board_value(vehicle_row, vehicle_col, 2);
                    int prev_row = vehicle_row + 1; 
                    int prev_col = vehicle_col;
                    _board->update_cell(vehicle_row, vehicle_col, 2, prev_row, prev_col);
                }
                break;
            case KEY_LEFT:
                if(vehicle_col > 0) {
                    change_inner_board_value(vehicle_row, vehicle_col, 0);
                    vehicle_col -= 1;
                    if(check_collision_from_vehicle(vehicle_row, vehicle_col)) {
                        stop_game();
                        break;
                    }
                    change_inner_board_value(vehicle_row, vehicle_col, 2);
                    int prev_row = vehicle_row; 
                    int prev_col = vehicle_col + 1;
                    _board->update_cell(vehicle_row, vehicle_col, 2, prev_row, prev_col);
                }       
                break;
            case KEY_DOWN:
                if(vehicle_row < _row - 1) {
                    change_inner_board_value(vehicle_row, vehicle_col, 0);
                    vehicle_row += 1;
                    if(check_collision_from_vehicle(vehicle_row, vehicle_col)) {
                        stop_game();
                        break;
                    }
                    change_inner_board_value(vehicle_row, vehicle_col, 2);
                    int prev_row = vehicle_row - 1; 
                    int prev_col = vehicle_col;
                    _board->update_cell(vehicle_row, vehicle_col, 2, prev_row, prev_col);
                }    
                break;  
            case KEY_RIGHT:
                if(vehicle_col < _col - 1) {
                    change_inner_board_value(vehicle_row, vehicle_col, 0);
                    vehicle_col += 1;
                    if(check_collision_from_vehicle(vehicle_row, vehicle_col)) {
                        stop_game();
                        break;
                    }
                    change_inner_board_value(vehicle_row, vehicle_col, 2);
                    int prev_row = vehicle_row; 
                    int prev_col = vehicle_col - 1;
                    _board->update_cell(vehicle_row, vehicle_col, 2, prev_row, prev_col);
                }    
                break;    
            default:
                break;
        }
    }

    change_inner_board_value(vehicle_row, vehicle_col, 0); 
    _board->empty_the_cell(vehicle_row, vehicle_col);
}

void Game::generate_obstacles() {
    while(!vehicle_created) {
        // wait till vehicle isn't created
        std::unique_lock<std::mutex> locker(_mutex);
        _cv.wait(locker);
    }   
    
    int gap_start = rand() % (_col - obstacle_gap);
    int gap_end = gap_start + (obstacle_gap - 1);

    while(game_should_go_on) {
        if(!game_should_go_on) break;
        gap_start = get_gap_start_index(gap_start);
        gap_end = gap_start + (obstacle_gap - 1);
        std::thread obs(&Game::generate_single_obstacle, this, gap_start, gap_end);
        obs.detach();
        std::this_thread::sleep_for(std::chrono::milliseconds(obstacle_stream_delay));
    }
}

void Game::generate_single_obstacle(int gap_start, int gap_end) {
    int row = 0;
    std::vector<int> cols;
    // init vector with all cols
    for(int c = 0; c < _col; c++) {
        if(c < gap_start || c > gap_end) {
            cols.push_back(c);
        }
    }
    // changing values & updating the cells
    change_inner_board_value(row, cols, 1);
    _board->update_cells(row, cols, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(obstacle_moving_delay));

    for(int r = 0; r < _row; r++) {    
        if(!game_should_go_on) {
            break;
        }
        if((r + 1) < _row && game_should_go_on) {
            if(check_collision_from_obstacle(r + 1, cols)) {
                stop_game();
                //break;
            }
            change_inner_board_value(r + 1, cols, 1);
            _board->update_cells(r + 1, cols, 1);
        }
        change_inner_board_value(r, cols, 0);
        _board->update_cells(r, cols, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(obstacle_moving_delay));
    }
    score++;
    //update_obstacle_delays();
}

int Game::get_gap_start_index(int prev_gap_start) {
    int jump = (obstacle_gap / 2);
    int new_start;
    if(gap_generation_direction) {
        new_start = prev_gap_start + jump;
        if(new_start + (obstacle_gap - 1) > _col) {
            // out of bounds
            new_start = prev_gap_start - jump;
            gap_generation_direction = 0;
        }
    } else {
        new_start = prev_gap_start - jump;
        if(new_start + (obstacle_gap - 1) < 0) {
            // out of bounds
            new_start = prev_gap_start + jump;
            gap_generation_direction = 1;
        }
    }
    return new_start;
}

bool Game::check_collision_from_obstacle(int row, std::vector<int> &cols) {
    for(int c = 0; c < cols.size(); c++) {
        if(get_inner_board_cell(row, cols.at(c)) == 2) { // obstacle found vehicle in this position
            return true;
        }
    }
    return false;
}

bool Game::check_collision_from_vehicle(int row, int col) {
    if(get_inner_board_cell(row, col) == 1) { // vehicle found obstacle in this position
        return true;
    }
    return false;
} 

void Game::update_obstacle_delays() {
    std::lock_guard<std::mutex> locker(_mutex);
    if(score <= 5) {
        obstacle_stream_delay = 2000;
        obstacle_moving_delay = 800;
    } else if(score > 5 && score <= 10) {
        obstacle_stream_delay = 1200;
        obstacle_moving_delay = 450;
    } else if(score > 10 && score <= 20) {
        obstacle_stream_delay = 1000;
        obstacle_moving_delay = 200;
    } else if(score > 20 && score <= 30) {
        obstacle_stream_delay = 500;
        obstacle_moving_delay = 100;
    } else {
        obstacle_stream_delay = 250;
        obstacle_moving_delay = 100;
    }
}

void Game::stop_game() {
    std::lock_guard<std::mutex> locker(_mutex);
    game_should_go_on = false;
}

void Game::change_inner_board_value(int b_row, int b_col, int val) {
    std::lock_guard<std::mutex> locker(_mutex);
    _inner_board[b_row][b_col] = val;
}

void Game::change_inner_board_value(int row, int val) {
    std::lock_guard<std::mutex> locker(_mutex);
    for(int c = 0; c < _col; c++) {
        _inner_board[row][c] = val;
    }
}

void Game::change_inner_board_value(int row, std::vector<int> &cols, int val) {
    std::lock_guard<std::mutex> locker(_mutex);
    for(int c = 0; c < cols.size(); c++) {
        _inner_board[row][cols.at(c)] = val;
    }
}

int Game::get_inner_board_cell(int row, int col) {
    return _inner_board[row][col];
}

void Game::post_game_over() {
    clear();
    attron(A_STANDOUT);
    printw("GAME OVER!\n");
    attroff(A_STANDOUT);
    attron(A_UNDERLINE);
    printw("Your score: %d\n", score);
    attroff(A_UNDERLINE);
    attron(A_DIM);
    printw("Press SPACE to exit!");
    attroff(A_DIM);
    int inp = getch();
    while(inp != 32) {  
        inp = getch(); 
    }
}

// dummy method for testing the matrix
void Game::print_inner_board() {
    std::lock_guard<std::mutex> locker(_mutex);
    for(int row=0; row<_row; row++) {
        for(int col=0; col<_col; col++) {
            mvprintw(row+30, col, "%d", _inner_board[row][col]);
        }
        printw("\n");
    }
    refresh();
}