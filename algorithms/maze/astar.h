#pragma once
#include "common.h"

#define ALGO_NAME "A*"

// A* step (implementation in astar.c)
bool astar_step(SearchState *s);

// Generic name used by maze renderer
#define step astar_step

