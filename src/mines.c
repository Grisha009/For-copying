#include "mines.h"
#include <stdlib.h>

void update_mines(Mine* mines, int mine_count) {
    for (int i = 0; i < mine_count; i++)
    {
        Mine *m = &mines[i];
        int new_x, new_y, base_x, base_y;
        
        switch (m->type)
        {
        case MINE_STATIC:
            break;
        case MINE_LR:
            new_x = m->pos.x + m->dm;

            if (new_x <= 1 || new_x >= BOARD_COLS - 2) {
                m->dm *= -1;
                new_x = m->pos.x + m->dm;
            }
            m->pos.x = new_x;
            break;
        case MINE_UD:
            new_y = m->pos.y + m->dm;

            if (new_y <= 1 || new_y >= BOARD_ROWS - 2) {
                m->dm *= -1;
                new_y = m->pos.y + m->dm;
            }
            m->pos.y = new_y;
            break;
        case MINE_LOOP_4:
            base_x = m->base_pos.x;
            base_y = m->base_pos.y;

            m->step = (m->step + 1) % 4;

            switch (m->step) {
                case 0: // top-left
                    m->pos.x = base_x;
                    m->pos.y = base_y;
                    break;
                case 1: // top-right
                    m->pos.x = base_x + 1;
                    m->pos.y = base_y;
                    break;
                case 2: // bottom-right
                    m->pos.x = base_x + 1;
                    m->pos.y = base_y + 1;
                    break;
                case 3: // bottom-left
                    m->pos.x = base_x;
                    m->pos.y = base_y + 1;
                    break;
                default:
                    break;
            }

            if (m->pos.x >= BOARD_COLS - 2) m->pos.x = BOARD_COLS - 3;
            if (m->pos.y >= BOARD_ROWS - 2) m->pos.y = BOARD_ROWS - 3;

            break;
        default:
            break;
        }
    }
    
}

int mine_at(Vec2 pos, Mine* mines, int mine_count) {
    for (int i = 0; i < mine_count; i++) {
        if (mines[i].pos.x == pos.x && mines[i].pos.y == pos.y) return 1;
    }
    return 0;
}


static int mine_conflicts(Vec2 pos, Mine* mines, int mine_count, Entity *robot, Entity *person) {
    if ((robot && pos.x == robot->pos.x && pos.y == robot->pos.y) ||
        (person && pos.x == person->pos.x && pos.y == person->pos.y)) {
        return 1;
    }
    for (int j = 0; j < mine_count; j++) {
        if (mines[j].pos.x == pos.x && mines[j].pos.y == pos.y) return 1;
    }
    return 0;
}

void spawn_mines(Mine* mines, int *mine_count, Entity *robot, Entity *person) {

    for (int i = 0; i < 5; i++)
    {
        Mine m;

        do {
            m.base_pos.x = rand() % (BOARD_COLS - 2) + 1;
            m.base_pos.y = rand() % (BOARD_ROWS - 2) + 1;
        } while (mine_conflicts(m.base_pos, mines, *mine_count, robot, person));
        m.pos = m.base_pos;

        if (i % 4 == 0) {
            m.type = MINE_LR;
            m.dm = (rand() % 2 == 0) ? 1 : -1;
            m.step = 0;
        }
        else if (i % 4 == 1) {
            m.type = MINE_LOOP_4;
            m.dm = (rand() % 2 == 0) ? 1 : -1;
            m.step = rand() % 4 ;
        }
        else if (i % 4 == 2) {
            m.type = MINE_UD;
            m.dm = (rand() % 2 == 0) ? 1 : -1;
            m.step = 0;
        } else {
            m.type = MINE_STATIC;
        }
        mines[*mine_count] = m;
        (*mine_count)++;
    }
    
}

void add_mines_for_level(Mine* mines, int *mine_count, int additional, Entity *robot, Entity *person) {
    for (int i = 0; i < additional && *mine_count < MAX_MINES; i++) {
        Mine m;
        do {
            m.base_pos.x = rand() % (BOARD_COLS - 2) + 1;
            m.base_pos.y = rand() % (BOARD_ROWS - 2) + 1;
        } while (mine_conflicts(m.base_pos, mines, *mine_count, robot, person));
        m.pos = m.base_pos;
        m.type = (i % 2 == 0) ? MINE_LOOP_4 : MINE_LR;
        m.dm = (rand() % 2 == 0) ? 1 : -1;
        m.step = rand() % 4;
        mines[*mine_count] = m;
        (*mine_count)++;
    }
}
