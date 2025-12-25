#pragma once
#include "common.h"

#define ALGO_NAME "Dijkstra"

// Dijkstra step (using min-heap, implementation in dijkstra.c)
bool dijkstra_step(SearchState *s);

// Generic name used by maze renderer
#define step dijkstra_step

