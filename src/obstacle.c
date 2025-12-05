#include "obstacle.h"
#include "consts.h"
#include <ncurses.h>
#include <stdlib.h>

int is_obstacle_position(Vec2 pos) {
    int center_x = BOARD_COLS / 2;
    int center_y = BOARD_ROWS / 2;
    if (pos.x == center_x && pos.y == center_y) return 0; // leave centre as safe respawn
    if ((abs(pos.x - center_x) <= 1 && abs(pos.y - center_y) <= 5) ||
        (abs(pos.y - center_y) <= 1 && abs(pos.x - center_x) <= 5)) {
        return 1;
    }
    return 0;
}

void draw_obstacle(GameWindows *gw) {
    WINDOW* win = gw->game_win;
    int center_x = BOARD_COLS / 2;
    int center_y = BOARD_ROWS / 2;
    for (int y = center_y - 5; y <= center_y + 5; y++) {
        for (int x = center_x - 5; x <= center_x + 5; x++) {
            Vec2 pos = {x, y};
            if (is_obstacle_position(pos)) {
                mvwaddch(win, pos.y, pos.x, OBSTACLE | COLOR_PAIR(4));
            }
        }
    }
}
