// Quick Sort (single-step, Lomuto partition with stack)
#pragma once

#include "common.h"

#define SORT_ALGO_NAME "Quick Sort"

// Initialization and one logical quick sort step (implemented in quick.c)
void quick_sort_init(SortState *s);
bool quick_sort_step(SortState *s);

// Generic names used by the sort visualizer
#define sort_init quick_sort_init
#define sort_step quick_sort_step
