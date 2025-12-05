#pragma once
#include "utils.h"
#include "consts.h"

typedef enum {
    MINE_STATIC,
    MINE_LR, // left-right
    MINE_UD, // up-down
    MINE_LOOP_4 // in a square loop
} MineType;

typedef struct {
    Vec2 pos;
    Vec2 base_pos;
    MineType type;
    int dm; // direction of movement
    int step;
} Mine;

void update_mines(Mine* mines, int mine_count);
void spawn_mines(Mine* mines, int *mine_count, Entity *robot, Entity *person);
Mine make_mine(Mine* mines, int *mine_count, Entity *robot, Entity *person);
void add_mines_for_level(Mine* mines, int *mine_count, int additional, Entity *robot, Entity *person);
int mine_at(Vec2 pos, Mine* mines, int mine_count);
