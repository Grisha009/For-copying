#include "robot.h"
#include <stdlib.h>

const Vec2 ROBOT_START = {10, 10};
const Vec2 ROBOT_RESPAWN = {10, 10};

static Vec2 direction_offset(Direction dir) {
    switch (dir) {
        case NORTH: return (Vec2){0, -1};
        case SOUTH: return (Vec2){0, 1};
        case EAST:  return (Vec2){1, 0};
        case WEST:  return (Vec2){-1, 0};
        default:    return (Vec2){0, 0};
    }
}

Vec2 body_position(Vec2 head, Direction dir) { // Quickly get body position
    Vec2 offset = direction_offset(dir);
    Vec2 body = {head.x - offset.x, head.y - offset.y};
    return body;
}

char head_char(Direction dir) { // Head char based on direction
    switch (dir) {
        case NORTH: return '^';
        case SOUTH: return 'v';
        case EAST:  return '>';
        case WEST:  return '<';
        default:    return ROBOT_HEAD;
    }
}

static int is_wall(Vec2 pos) {
    return pos.x <= 0 || pos.x >= BOARD_COLS - 1 || pos.y <= 0 || pos.y >= BOARD_ROWS - 1;
}

static void wait_for_continue(GameWindows *gw, const char *msg) { // Pause the game until a key is pressed
    nodelay(stdscr, FALSE);
    mvwprintw(gw->ui_win, 3, 1, "%-70s", "");
    mvwprintw(gw->ui_win, 3, 1, "%s", msg);
    wrefresh(gw->ui_win);
    getch();
    nodelay(stdscr, TRUE);
}

void handle_input(int ch, GameState *state, Entity *player) { // Input handling
    (void)player;
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
        case 'm':
        case 'M':
            state->ai_mode = !state->ai_mode;
            break;
        default:
            break;
    }
}

static void reset_robot(Entity *robot, GameState *state, GameWindows *gw) { // Called upon collisions
    if (state->shield_turns > 0) return;
    state->lives--;
    state->dir = NONE;
    robot->pos = ROBOT_RESPAWN;
    wait_for_continue(gw, "Collision! Press any key to continue...");
}

void move_robot(Direction dir, Entity *robot, GameState *state, Mine* mines, int mine_count, GameWindows *gw) { // Manual movement
    if (dir == NONE) return;

    Vec2 delta = direction_offset(dir);
    Vec2 new_head = {robot->pos.x + delta.x, robot->pos.y + delta.y};
    Vec2 new_body = body_position(new_head, dir);

    // If collided, reset robot unless shield is active
    if (is_wall(new_head) || is_obstacle_position(new_head) || mine_at(new_head, mines, mine_count) ||
        is_wall(new_body) || is_obstacle_position(new_body) || mine_at(new_body, mines, mine_count)) {
        if (state->shield_turns > 0) {
            robot->pos = new_head;
            return;
        }
        reset_robot(robot, state, gw);
        return;
    }
    robot->pos = new_head;
}

static int distance(Vec2 a, Vec2 b) { // square of distance between two points
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// AI movement
void move_robot_ai(Entity *robot, Entity *person, Mine* mines, int mine_count, GameState *state, GameWindows *gw) {
    Direction candidates[4] = {NORTH, SOUTH, EAST, WEST};
    Direction best_dir = NONE;
    int best_score = 100000;
    for (int i = 0; i < 4; i++) {
        // Check score for each direction
        Direction dir = candidates[i];
        Vec2 delta = direction_offset(dir); // transfer direction into Vec2
        Vec2 target = {robot->pos.x + delta.x, robot->pos.y + delta.y}; // potential position
        Vec2 target_body = body_position(target, dir);
        if (is_wall(target) || is_obstacle_position(target) || mine_at(target, mines, mine_count) ||
            is_wall(target_body) || is_obstacle_position(target_body) || mine_at(target_body, mines, mine_count)) continue;
        int dist = distance(target, person->pos);
        if (dist < best_score) {
            best_score = dist;
            best_dir = dir;
        }
    }
    if (best_dir == NONE) return;
    state->dir = best_dir;
    move_robot(best_dir, robot, state, mines, mine_count, gw);
}

void check_collision(Entity *robot, GameState *state, Mine *mines, int mine_count, GameWindows *gw) {
    if (state->shield_turns > 0) return;
    Vec2 body = body_position(robot->pos, state->dir);
    if (mine_at(robot->pos, mines, mine_count) || mine_at(body, mines, mine_count) ||
        is_obstacle_position(robot->pos) || is_obstacle_position(body)) {
        reset_robot(robot, state, gw);
    }
}
