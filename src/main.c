#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"
#include "mines.h"
#include "consts.h"


/* ------------ Function declarations ------------ */


// Title + Game UI
void draw_title_screen(void);
void draw_ui(GameWindows *gw, GameState* state);
void draw_game(GameWindows *gw, Entity *player, Mine* mines, int mine_count);

// Updating
void handle_input(int ch, GameState *state, Entity *player);
void move_robot(Direction dir, Entity *robot, GameState *state);

// Logic
void collision_detection();


void draw_title_screen(void)
{
    mvprintw(0, 10, "ROBOT SAVE THE PEOPLE GAME");
    mvprintw(0, 10, "Instructions:\n- Chase the person while avoiding mines (X)\n- Avoid the cross-shaped obstacle (#)\n- Catch the person to score points");
    refresh();
}

/* ------------ Function implementations ------------ */



int main(void) {    
    // ----------------------- INITIAL GAME SETUP -----------------------------
    GameWindows gw = {0};
    GameState state = {.score = 0, .level = 1, .lives = 3, .running = 1, .player_name="Bob", .dir = NONE};
    Entity player = {.pos = {1, 1}, .glyph = ROBOT_BODY};
    Entity person = {.pos = {10, 10}, .glyph = PERSON};
    Mine mines[MAX_MINES] = {0};
    int mine_count = 0;
    srand((unsigned int)time(NULL));
    init_mines(mines, &mine_count);

    // ----------------------- INITIAL WINDOW SETUP ---------------------------
    init_ncurses();
    create_windows(&gw);

    // -------------------------- GAME LOOP -----------------------------------
    while (state.running) {
        int ch = getch();
        
        // Updating
        handle_input(ch, &state, &player);
        move_robot(state.dir, &player, &state);
        update_mines(mines, mine_count);

        // Drawing
        draw_ui(&gw, &state);
        draw_game(&gw, &player, mines, mine_count);


        // Update logic
        if (state.lives <= 0) state.running = false;
        
        napms(60);
        
    }

    shutdown_ncurses(&gw);
    return 0;
}

void handle_input(int ch, GameState *state, Entity *player) {
    switch(ch) {
        case KEY_UP:
            state->dir = NORTH;
            break;
        case KEY_DOWN:
            state->dir = SOUTH;
            break;
        case KEY_LEFT:
            state->dir = WEST;
            break;
        case KEY_RIGHT:
            state->dir = EAST;
            break;
        case 'q':
        case 'Q':
            state->running = 0;
            break;
        default:
            break;
    }
}

void move_robot(Direction dir, Entity *robot, GameState *state) {
    if (dir != NONE) {
        int new_y = robot->pos.y;
        int new_x = robot->pos.x;
        switch (dir) {
            case NORTH:
                new_y--;
                break;
            case SOUTH:
                new_y++;
                break;
            case EAST:
                new_x++;
                break;
            case WEST:
                new_x--;
                break;
            case NONE:
                break;
            default:
                break;
        }

        // Bounds checking:

        if (new_y < 1 || new_y > BOARD_ROWS - 2 || new_x < 1 || new_x > BOARD_COLS - 2) {
            state->lives--;
            state->dir = NONE;
            robot->pos.x = 10;
            robot->pos.y = 10;
            return;
        }
        robot->pos.x = new_x;
        robot->pos.y = new_y;
    }
}

void draw_ui(GameWindows *gw, GameState* state) {
    WINDOW* win = gw->ui_win;
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 1, 1, "Player: %s", state->player_name);
    mvwprintw(win, 2, 1, "Level: %d", state->level);
    mvwprintw(win, 2, 11, "Lives: %d", state->lives);
    mvwprintw(win, 2, 21, "Score: %d", state->score);
    wrefresh(win);
}

void draw_game(GameWindows *gw, Entity *player, Mine* mines, int mine_count) {
    WINDOW* win = gw->game_win;
    werase(win); // reset the window
    box(win, 0, 0);

    // Draw robot
    mvwaddch(win, player->pos.y, player->pos.x, player->glyph);

    // Draw mines
    for (int i = 0; i < mine_count; i++) {
        mvwaddch(win, mines[i].pos.y, mines[i].pos.x, MINE);
    }

    wrefresh(win);
}
