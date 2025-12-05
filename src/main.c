#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"
#include "mines.h"
#include "robot.h"
#include "obstacle.h"
#include "consts.h"

/* ------------ Constants ------------ */

static const char *LEADERBOARD_PATH = "leaderboard.txt";

/* ------------ Helper declarations ------------ */

void draw_title_screen(GameState *state);
void update_UI(GameWindows *gw, GameState* state);
void draw_game(GameWindows *gw, Entity *player, Entity *person, Entity *powerup, Mine* mines, int mine_count, GameState *state);
void game_over_screen(GameWindows *gw, GameState *state);
void show_leaderboard(int start_row);

Vec2 spawn_person(Entity *robot, Mine *mines, int mine_count);
void handle_person_collection(Entity *robot, Entity *person, Entity *powerup, GameState *state, Mine *mines, int *mine_count);
void maybe_spawn_powerup(Entity *powerup, Entity *robot, Mine *mines, int mine_count);
void save_score(GameState *state);
void draw_title_screen(GameState *state);

int main(void) {    
    srand((unsigned int)time(NULL));

    // ----------------------- INITIAL GAME SETUP -----------------------------
    GameWindows gw = {0};
    GameState state = {.score = 0, .level = 1, .lives = 3, .running = 1, .player_name="Bob", .dir = EAST, .ai_mode = 0, .tick_delay = BASE_TICK_DELAY, .shield_turns = 0};
    Entity player = {.pos = ROBOT_START, .glyph = ROBOT_BODY};
    Entity person = {.pos = {0, 0}, .glyph = PERSON};
    Entity powerup = {.pos = {-1, -1}, .glyph = POWERUP};
    Mine mines[MAX_MINES] = {0};
    int mine_count = 0;
    
    // ----------------------- INITIAL WINDOW SETUP ---------------------------
    init_ncurses();
    draw_title_screen(&state);
    create_windows(&gw);
    spawn_mines(mines, &mine_count, &player, NULL);
    person.pos = spawn_person(&player, mines, mine_count);

    // -------------------------- GAME LOOP -----------------------------------
    while (state.running && state.lives > 0) {
        int ch = getch();
        
        // Updating
        handle_input(ch, &state, &player);
        if (state.ai_mode) {
            move_robot_ai(&player, &person, mines, mine_count, &state, &gw);
        } else {
            move_robot(state.dir, &player, &state, mines, mine_count, &gw);
        }

        if (state.running == 0) break;

        update_mines(mines, mine_count);
        check_collision(&player, &state, mines, mine_count, &gw);

        // Drawing
        update_UI(&gw, &state);
        draw_game(&gw, &player, &person, &powerup, mines, mine_count, &state);

        // Update logic
        handle_person_collection(&player, &person, &powerup, &state, mines, &mine_count);
        if (powerup.pos.x == player.pos.x && powerup.pos.y == player.pos.y) {
            state.shield_turns = 20;
            powerup.pos.x = -1;
            powerup.pos.y = -1;
        }
        if (state.shield_turns > 0) state.shield_turns--;

        if (state.lives <= 0) state.running = 0;
        
        napms(state.tick_delay);
        
    }

    save_score(&state);
    game_over_screen(&gw, &state);

    shutdown_ncurses(&gw);
    return 0;
}

Vec2 spawn_person(Entity *robot, Mine *mines, int mine_count) {
    Vec2 pos;
    do {
        pos.x = rand() % (BOARD_COLS - 2) + 1;
        pos.y = rand() % (BOARD_ROWS - 2) + 1;
    } while ((pos.x == robot->pos.x && pos.y == robot->pos.y) ||
             mine_at(pos, mines, mine_count) ||
             is_obstacle_position(pos));
    return pos;
}

void handle_person_collection(Entity *robot, Entity *person, Entity *powerup, GameState *state, Mine *mines, int *mine_count) {
    if (robot->pos.x == person->pos.x && robot->pos.y == person->pos.y) {
        state->score++;
        person->pos = spawn_person(robot, mines, *mine_count);
        maybe_spawn_powerup(powerup, robot, mines, *mine_count);
        if (state->score % 5 == 0) {
            state->level++;
            add_mines_for_level(mines, mine_count, 2, robot, person);
            int delay = BASE_TICK_DELAY - (state->level - 1) * 2;
            if (delay < 20) delay = 20;
            state->tick_delay = delay;
        }
    }
}

void maybe_spawn_powerup(Entity *powerup, Entity *robot, Mine *mines, int mine_count) {
    if (powerup->pos.x != -1) return; // Do not move the current powerup if there is one
    if ((rand() % 5) == 0) {
        Vec2 pos;
        // Generate position until it's free
        do {
            pos.x = rand() % (BOARD_COLS - 2) + 1;
            pos.y = rand() % (BOARD_ROWS - 2) + 1;
        } while (is_obstacle_position(pos) || mine_at(pos, mines, mine_count) ||
                 (pos.x == robot->pos.x && pos.y == robot->pos.y));
        powerup->pos = pos;
    }
}

void update_UI(GameWindows *gw, GameState* state) {
    WINDOW* win = gw->ui_win;
    werase(win);
    box(win, 0, 0);
    // Print game state
    mvwprintw(win, 1, 1, "Player: %s", state->player_name);
    mvwprintw(win, 2, 1, "Level: %d", state->level);
    mvwprintw(win, 2, 11, "Lives: %d", state->lives);
    mvwprintw(win, 2, 21, "Score: %d", state->score);
    mvwprintw(win, 2, 31, "Mode: %s", state->ai_mode ? "AI" : "Manual");
    if (state->shield_turns > 0) {
        mvwprintw(win, 1, 25, "Shield: %d", state->shield_turns);
    }
    wrefresh(win);
}

void draw_game(GameWindows *gw, Entity *player, Entity *person, Entity *powerup, Mine* mines, int mine_count, GameState *state) {
    WINDOW* win = gw->game_win;
    werase(win); // reset the window
    box(win, 0, 0);

    // Draw obstacles
    draw_obstacle(gw);

    // Draw the person
    mvwaddch(win, person->pos.y, person->pos.x, person->glyph | COLOR_PAIR(2));

    // Draw mines
    for (int i = 0; i < mine_count; i++) {
        mvwaddch(win, mines[i].pos.y, mines[i].pos.x, MINE | COLOR_PAIR(3));
    }

    // Draw powerup (if active)
    if (powerup->pos.x >= 0) {
        mvwaddch(win, powerup->pos.y, powerup->pos.x, powerup->glyph | COLOR_PAIR(5));
    }

    // Draw robot body and head
    Direction render_dir = (state->dir == NONE) ? EAST : state->dir;
    Vec2 body = body_position(player->pos, render_dir);
    mvwaddch(win, body.y, body.x, ROBOT_BODY | COLOR_PAIR(1));
    mvwaddch(win, player->pos.y, player->pos.x, head_char(render_dir) | COLOR_PAIR(1));

    wrefresh(win);
}

void game_over_screen(GameWindows *gw, GameState *state) {
    destroy_windows(gw);
    clear();
    mvprintw(5, 10, "GAME OVER, %s!", state->player_name);
    mvprintw(7, 10, "Final Score: %d  Level: %d", state->score, state->level);
    mvprintw(9, 10, "Leaderboard:");
    show_leaderboard(10);
    mvprintw(18, 10, "Press any key to exit.");
    refresh();
    nodelay(stdscr, FALSE);
    getch();
}

void save_score(GameState *state) {
    FILE *file = fopen(LEADERBOARD_PATH, "a");
    if (!file) return;
    fprintf(file, "%s %d\n", state->player_name, state->score);
    fclose(file);
}

void show_leaderboard(int start_row) {
    FILE *file = fopen(LEADERBOARD_PATH, "r");
    if (!file) {
        mvprintw(start_row, 6, "No scores yet.");
        return;
    }
    char name[32];
    int score;
    int row = start_row;
    mvprintw(row++, 6, "%-16s %6s", "Name", "Score");
    while (fscanf(file, "%31s %d", name, &score) == 2 && row < start_row + 6) {
        mvprintw(row, 6, "%-16s %6d", name, score);
        row++;
    }
    fclose(file);
}

void draw_title_screen(GameState *state)
{
    clear();
    mvprintw(1, 10, "ROBOT SAVE THE PEOPLE GAME");
    mvprintw(2, 4, "Instructions:");
    mvprintw(3, 6, "- Chase the person (P) while avoiding mines (X) and obstacles (#)");
    mvprintw(4, 6, "- The robot has a body (O) and a head (>), make sure you avoid collisions for both of them!");
    mvprintw(5, 6, "- Press 'm' to switch between manual and AI controls");
    mvprintw(6, 6, "- Press 'q' to quit");
    mvprintw(7, 6, "- The power-up (*) grants a short shield that saves you from all collisions");
    mvprintw(8, 4, "Enter your name: ");
    // Get players name
    echo();
    nodelay(stdscr, FALSE);
    getnstr(state->player_name, (int)(sizeof(state->player_name) - 1));
    // Go back to game mode
    noecho();
    nodelay(stdscr, TRUE);
    if (strlen(state->player_name) == 0) {
        strncpy(state->player_name, "Player", sizeof(state->player_name) - 1);
    }
    mvprintw(12, 4, "Leaderboard:");
    show_leaderboard(13);
    mvprintw(18, 4, "Press any key to start...");
    refresh();
    getch();
    clear();
    refresh();
}