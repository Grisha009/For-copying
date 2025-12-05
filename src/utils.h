#pragma once
#include <ncurses.h>

typedef enum  {
    NORTH,
    SOUTH,
    EAST,
    WEST,
    NONE
} Direction;

typedef struct {
    int x;
    int y;
} Vec2;

typedef struct {
    WINDOW *ui_win;
    WINDOW *game_win;
    int screen_rows;
    int screen_cols;
} GameWindows;

typedef struct {
    char player_name[32];
    int score;
    int lives;
    int level;
    int running; // 1 = game running, 0 - not running
    Direction dir;
    int ai_mode; // 0 manual, 1 AI
    int tick_delay; // milliseconds between frames
    int shield_turns; // creative enhancement: temporary shield duration
} GameState;

typedef struct {
    Vec2 pos;
    char glyph;
} Entity;

void init_ncurses(void);
void shutdown_ncurses(GameWindows *gw);

void create_windows(GameWindows *gw);
void destroy_windows(GameWindows *gw);
