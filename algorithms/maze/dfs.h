#pragma once
#include "common.h"

#define ALGO_NAME "DFS"

// Depth-first search step (implementation in dfs.c)
bool dfs_step(SearchState *s);

// Generic name used by maze renderer
#define step dfs_step

