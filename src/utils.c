#include "utils.h"
#include "consts.h"
#include <stdlib.h>
#include <time.h>

void init_ncurses(void) {
    initscr();              // Start ncurses mode
    cbreak();               // Disable line buffering
    noecho();               // Don't echo typed characters
    keypad(stdscr, TRUE);   // Enable arrow keys
    curs_set(0);            // Hide cursor
    srand(time(NULL));
    nodelay(stdscr, TRUE);
}

void shutdown_ncurses(GameWindows *gw) {
    destroy_windows(gw);
    endwin();
}

void create_windows(GameWindows *gw) {
    // Get terminal size
    getmaxyx(stdscr, gw->screen_rows, gw->screen_cols);

    // UI window
    gw->ui_win = newwin(
        UI_HEIGHT,
        gw->screen_cols,
        0,
        0
    );

    // Game window
    gw->game_win = newwin(
        BOARD_ROWS,
        BOARD_COLS,
        UI_HEIGHT + 1,
        (gw->screen_cols - BOARD_COLS) / 2
    );

    // Borders around both windows
    box(gw->ui_win, 0, 0);
    box(gw->game_win, 0, 0);

    wrefresh(gw->ui_win);
    wrefresh(gw->game_win);
}

void destroy_windows(GameWindows *gw) {
    if (gw->ui_win) {
        delwin(gw->ui_win);
        gw->ui_win = NULL;
    }
    if (gw->game_win) {
        delwin(gw->game_win);
        gw->game_win = NULL;
    }
}
