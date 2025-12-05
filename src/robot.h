#pragma once
#include <ncurses.h>
#include "utils.h"
#include "mines.h"
#include "obstacle.h"

extern const Vec2 ROBOT_START;
extern const Vec2 ROBOT_RESPAWN;

void handle_input(int ch, GameState *state, Entity *player);
void move_robot(Direction dir, Entity *robot, GameState *state, Mine* mines, int mine_count, GameWindows *gw);
void move_robot_ai(Entity *robot, Entity *person, Mine* mines, int mine_count, GameState *state, GameWindows *gw);
void check_collision(Entity *robot, GameState *state, Mine *mines, int mine_count, GameWindows *gw);
Vec2 body_position(Vec2 head, Direction dir);
char head_char(Direction dir);
