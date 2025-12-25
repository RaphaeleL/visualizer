#pragma once
#include "../../libs/build.h"

// Maze cell types and search utilities shared by all algorithms.

#define WALL 1
#define PATH 0
#define INF  1000000

// 4-neighborhood directions (up, right, down, left).
extern int dirs[4][2];

typedef struct {
    int x;
    int y;
} Cell;

typedef qol_list(Cell) CellList;

typedef struct {
    int N;
    int **maze;
    int startX, startY, goalX, goalY;

    int max;
    int *visited;
    int *parent;
    int *dist;
    int *processed;
    int *fscore;     // used by A* variants

    // Min-heap for priority-based searches (indexes into grid)
    int *heap;
    int *heap_pos;
    int heap_len;

    CellList queue; // used by BFS/DFS and for discovered nodes
    int head;       // BFS head index

    CellList path;  // filled when goal found
} SearchState;

// Initialize / clean up search state and rebuild the final path.
void search_init(SearchState *s, int N, int **maze, int sx, int sy, int gx, int gy);
int  build_path(SearchState *s);
void search_free(SearchState *s);

