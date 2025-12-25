// Selection Sort (single-step)
#pragma once

#include "common.h"

#define SORT_ALGO_NAME "Selection Sort"

// Initialization and one logical selection sort step (implemented in selection.c)
void selection_sort_init(SortState *s);
bool selection_sort_step(SortState *s);

// Generic names used by the sort visualizer
#define sort_init selection_sort_init
#define sort_step selection_sort_step

