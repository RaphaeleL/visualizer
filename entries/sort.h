#pragma once
#include <raylib.h>

#define SORT_TICK 0.0f
#define SORT_MAX_VALUE 420

#include "../algorithms/sort/common.h"

// Choose one algorithm (comment/uncomment a single include):
// #include "../algorithms/sort/bubble.h"
// #include "../algorithms/sort/selection.h"
// #include "../algorithms/sort/merge.h"
#include "../algorithms/sort/quick.h"
// #include "../algorithms/sort/heap.h"

// Public functions
void sort(void);
