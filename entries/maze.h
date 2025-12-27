#pragma once
#include <raylib.h>

// Maze generation using recursive backtracking
#define TICK 0.025f // seconds per step
#define SEED -1 // -1 for random seed

#include "../algorithms/maze/common.h"

// Choose one algorithm:
// #include "../algorithms/maze/bfs.h"
// #include "../algorithms/maze/dfs.h"
// #include "../algorithms/maze/greedy.h"
// #include "../algorithms/maze/astar.h"
#include "../algorithms/maze/dijkstra.h"

// Public functions
void maze(void);
