#pragma once
#include "common.h"

#define ALGO_NAME "Greedy"

// Greedy best-first step (implementation in greedy.c)
bool greedy_step(SearchState *s);

// Generic name used by maze renderer
#define step greedy_step

