// Bottom-up Merge Sort (single-step)
#pragma once

#include "common.h"

#define SORT_ALGO_NAME "Merge Sort"

// Initialization and one logical merge sort step (implemented in merge.c)
void merge_sort_init(SortState *s);
bool merge_sort_step(SortState *s);

// Generic names used by the sort visualizer
#define sort_init merge_sort_init
#define sort_step merge_sort_step

