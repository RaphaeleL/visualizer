#pragma once
#include "common.h"

#define ALGO_NAME "BFS"

// Breadth-first search step (implementation in bfs.c)
bool bfs_step(SearchState *s);

// Generic name used by maze renderer
#define step bfs_step

