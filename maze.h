#pragma once
#include <raylib.h>

#define QOL_IMPLEMENTATION
#include "libs/build.h"

// Maze generation using recursive backtracking
#define WALL 1
#define PATH 0

#define INF 1000000
#define TICK 0.025f // seconds per step
#define SEED -1 // -1 for random seed

static int dirs[4][2] = {{0,-1},{1,0},{0,1},{-1,0}};

#include "algorithms/maze/common.h"

// Choose one algorithm:
// #include "algorithms/maze/bfs.h"
// #include "algorithms/maze/dfs.h"
// #include "algorithms/maze/greedy.h"
// #include "algorithms/maze/astar.h"
#include "algorithms/maze/dijkstra.h"

void ShuffleDirs() {
    for (int i = 0; i < 4; i++) {
        int r = rand() % 4;
        int tmp0 = dirs[i][0];
        int tmp1 = dirs[i][1];
        dirs[i][0] = dirs[r][0];
        dirs[i][1] = dirs[r][1];
        dirs[r][0] = tmp0;
        dirs[r][1] = tmp1;
    }
}

void GenerateMaze(int **maze, int x, int y, int n) {
    maze[y][x] = PATH;
    ShuffleDirs();

    for (int i = 0; i < 4; i++) {
        int nx = x + dirs[i][0] * 2;
        int ny = y + dirs[i][1] * 2;

        if (nx > 0 && nx < n-1 && ny > 0 && ny < n-1) {
            if (maze[ny][nx] == WALL) {
                maze[y + dirs[i][1]][x + dirs[i][0]] = PATH;
                GenerateMaze(maze, nx, ny, n);
            }
        }
    }
}

static void ResetRun(
    int **maze,
    int N,
    int *startX,
    int *startY,
    int *goalX,
    int *goalY,
    SearchState *state,
    bool *found,
    int *pathLen,
    float *tickTime,
    double *timeFound,
    int *stepCount,
    QOL_Timer *searchTimer
) {
    // clear and regenerate maze
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            maze[y][x] = WALL;
        }
    }
    GenerateMaze(maze, 1, 1, N);

    // pick new start/goal on PATH cells
    do {
        *startX = rand() % N;
        *startY = rand() % N;
    } while (maze[*startY][*startX] == WALL);

    do {
        *goalX = rand() % N;
        *goalY = rand() % N;
    } while ((*goalX == *startX && *goalY == *startY) || maze[*goalY][*goalX] == WALL);

    // free previous search buffers if any, then reinit
    if (state->visited) {
        search_free(state);
    }
    *state = (SearchState){0};
    search_init(state, N, maze, *startX, *startY, *goalX, *goalY);

    // reset runtime stats/timers
    *found = false;
    *pathLen = 0;
    *tickTime = 0.0f;
    *timeFound = 0.0;
    *stepCount = 0;
    qol_timer_start(searchTimer);
}

void maze(void) {
    const int N = 31;           // must be odd
    const int CELL = 20;        // cell size in pixels
    const int SCREEN = N * CELL;

    unsigned int seed_value = (SEED == -1) ? (unsigned int)time(NULL) : (unsigned int)SEED;
    srand(seed_value);

    // Allocate maze
    int **maze = malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++) {
        maze[i] = malloc(N * sizeof(int));
        for (int j = 0; j < N; j++) maze[i][j] = WALL;
    }

    // Generate maze
    int startX, startY, goalX, goalY;
    SearchState state = {0};
    bool found = false;
    int pathLen = 0;
    float tickTime = 0.0f;
    double timeFound = 0.0;
    int stepCount = 0;   // number of search steps performed
    QOL_Timer searchTimer;
    ResetRun(maze, N, &startX, &startY, &goalX, &goalY, &state, &found, &pathLen, &tickTime, &timeFound, &stepCount, &searchTimer);

    // Persistent colors
    const Color startColor = YELLOW;
    const Color goalColor = BLUE;
    const Color pathColor = RED;

    InitWindow(SCREEN, SCREEN, TextFormat("Random Maze with %s", ALGO_NAME));
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            if (IsKeyPressed(KEY_R)) {
                ResetRun(maze, N, &startX, &startY, &goalX, &goalY, &state, &found, &pathLen, &tickTime, &timeFound, &stepCount, &searchTimer);
            }
            ClearBackground(BLACK);

            // Draw maze
            for (int y = 0; y < N; y++) {
                for (int x = 0; x < N; x++) {
                    Color c = (maze[y][x] == WALL) ? BLACK : RAYWHITE;
                    DrawRectangle(x * CELL, y * CELL, CELL, CELL, c);
                }
            }

            // Visualize visited/search frontier
            for (int i = 0; i < N * N; i++) {
                if (state.visited[i]) {
                    int x = i % N;
                    int y = i / N;
                    if (!((x == startX && y == startY) || (x == goalX && y == goalY))) {
                        DrawRectangle(x * CELL, y * CELL, CELL, CELL, Fade(pathColor, 0.2f));
                    }
                }
            }

            // Incremental expansion: one cell every tick
            float frameTime = GetFrameTime();
            tickTime += frameTime;
            if (!found && tickTime >= TICK) {
                tickTime = 0.0f;
                if (step(&state)) {
                    found = true;
                    pathLen = (int)state.path.len;
                    timeFound = qol_timer_elapsed(&searchTimer);
                    stepCount++;
                } else {
                    stepCount++;
                }
            }

            // Draw path once found
            if (found) {
                for (int i = 0; i < pathLen; i++) {
                    int x = state.path.data[i].x;
                    int y = state.path.data[i].y;
                    if (!((x == startX && y == startY) || (x == goalX && y == goalY))) {
                        DrawRectangle(x * CELL, y * CELL, CELL, CELL, pathColor);
                    }
                }
            }

            // Keep start/goal cells filled with their colors
            DrawRectangle(startX * CELL, startY * CELL, CELL, CELL, startColor);
            DrawRectangle(goalX * CELL, goalY * CELL, CELL, CELL, goalColor);
            DrawCircle(startX * CELL + CELL/2, startY * CELL + CELL/2, CELL/3, startColor);
            DrawCircle(goalX * CELL + CELL/2, goalY * CELL + CELL/2, CELL/3, goalColor);

            // Info popup when goal is reached
            if (found) {
                int visitedCount = 0;
                for (int i = 0; i < N * N; i++) {
                    if (state.visited[i]) visitedCount++;
                }

                const int panelW = 340;
                const int panelH = 170;
                const int panelX = (SCREEN - panelW) / 2;
                const int panelY = 20;
                DrawRectangle(panelX, panelY, panelW, panelH, Fade(BLACK, 0.8f));
                DrawRectangleLines(panelX, panelY, panelW, panelH, RAYWHITE);

                char buf[128];
                int lineY = panelY + 12;
                DrawText("Result (press r to rerun)", panelX + 10, lineY, 20, RAYWHITE); lineY += 24;

                snprintf(buf, sizeof(buf), "algo: %s", ALGO_NAME);
                DrawText(buf, panelX + 10, lineY, 18, YELLOW); lineY += 20;

                snprintf(buf, sizeof(buf), "time: %.3fs", timeFound);
                DrawText(buf, panelX + 10, lineY, 18, YELLOW); lineY += 20;

                snprintf(buf, sizeof(buf), "tick: %.3fs", (double)TICK);
                DrawText(buf, panelX + 10, lineY, 18, YELLOW); lineY += 20;

                double netTime = timeFound - (stepCount * (double)TICK);
                if (netTime < 0) netTime = 0;
                snprintf(buf, sizeof(buf), "net time(no tick): %.3fs", netTime);
                DrawText(buf, panelX + 10, lineY, 18, YELLOW); lineY += 20;

                snprintf(buf, sizeof(buf), "path len: %d", pathLen);
                DrawText(buf, panelX + 10, lineY, 18, YELLOW); lineY += 20;

                snprintf(buf, sizeof(buf), "visited: %d", visitedCount);
                DrawText(buf, panelX + 10, lineY, 18, YELLOW);
            }

        EndDrawing();
    }

    CloseWindow();

    for (int i = 0; i < N; i++) free(maze[i]);
    free(maze);
    if (state.visited) search_free(&state);
}
