// Heap Sort (single-step)
#pragma once

#include "common.h"

#define SORT_ALGO_NAME "Heap Sort"

// Initialization and one logical heap sort step (implemented in heap.c)
void heap_sort_init(SortState *s);
bool heap_sort_step(SortState *s);

// Generic names used by the sort visualizer
#define sort_init heap_sort_init
#define sort_step heap_sort_step

