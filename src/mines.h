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
void init_mines(Mine* mines, int *mine_count);