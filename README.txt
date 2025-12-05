Robot Save The People – Game

Overview
- Terminal-based robot rescue game built with C and ncurses.
- Robot collects people, avoids mines and a central obstacle, levels up over time, and records scores.

Build (macOS/Linux)
- Prerequisite: ncurses development headers (macOS: Xcode Command Line Tools; Ubuntu: sudo apt-get install libncurses5-dev libncursesw5-dev).
- From the project root: `gcc -Wall -Wextra -pedantic src/*.c -lncurses -o robot_game`

Run
- `./robot_game`
- A `leaderboard.txt` file in the project root stores scores; it is created automatically if missing.

Controls
- Arrow keys: Move (Manual mode keeps moving in last direction).
- m: Toggle Manual/AI modes.
- q: Quit.

Core Features
- Manual and AI movement; AI uses BFS pathfinding that avoids walls, mines, and the obstacle.
- Robot drawn with body + directional head; colored rendering for robot, person, mines, obstacle, power-up.
- Central cross obstacle; colliding costs a life and respawns the robot.
- Mines spawn and move; +2 mines each level, up to 50.
- People spawn one at a time; collecting increases score; every 5 people increases level and speed.
- Power-up (*) grants temporary shield against collisions.
- Title screen with instructions and name entry; HUD shows name, score, level, lives, mode, shield.
- Game-over screen with leaderboard display; scores appended to `leaderboard.txt`.

Files
- src/main.c: Game loop, UI screens, spawning, scoring, leaderboard.
- src/robot.c/.h: Input handling, manual/AI movement, collisions, pathfinding helpers.
- src/mines.c/.h: Mine spawning and movement.
- src/obstacle.c/.h: Obstacle layout and detection.
- src/utils.c/.h: ncurses/window setup, shared types/constants.
- src/consts.h: Board, glyph, and timing constants.
- leaderboard.txt: Saved scores (text).
