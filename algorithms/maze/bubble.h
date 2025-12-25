// Bubble Sort (single-step)
#pragma once

#include "../sort/common.h"

#define SORT_ALGO_NAME "Bubble Sort"

// Initialization and one logical bubble sort step (implemented in bubble.c)
void bubble_sort_init(SortState *s);
bool bubble_sort_step(SortState *s);

// Generic names used by the sort visualizer (when included there)
#define sort_init bubble_sort_init
#define sort_step bubble_sort_step

