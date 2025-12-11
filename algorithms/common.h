#pragma once
#include "../libs/build.h"

typedef struct {
    int x;
    int y;
} Cell;

typedef list(Cell) CellList;

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

static inline void search_init(SearchState *s, int N, int **maze, int sx, int sy, int gx, int gy) {
    s->N = N;
    s->maze = maze;
    s->startX = sx;
    s->startY = sy;
    s->goalX = gx;
    s->goalY = gy;
    s->max = N * N;

    s->visited = calloc(s->max, sizeof(int));
    s->parent = malloc(s->max * sizeof(int));
    s->dist = malloc(s->max * sizeof(int));
    s->processed = calloc(s->max, sizeof(int));
    s->fscore = malloc(s->max * sizeof(int));
    s->heap = malloc(s->max * sizeof(int));
    s->heap_pos = malloc(s->max * sizeof(int));
    for (int i = 0; i < s->max; i++) {
        s->parent[i] = -1;
        s->dist[i] = INF;
        s->fscore[i] = INF;
        s->heap_pos[i] = -1;
    }
    s->heap_len = 0;

    s->queue.len = s->queue.cap = 0;
    s->queue.data = NULL;
    s->path.len = s->path.cap = 0;
    s->path.data = NULL;

    push(&s->queue, ((Cell){sx, sy}));
    s->head = 0;
    s->visited[sy * N + sx] = 1;
    s->dist[sy * N + sx] = 0;
}

static inline int build_path(SearchState *s) {
    s->path.len = 0;
    int cx = s->goalX;
    int cy = s->goalY;

    while (!(cx == s->startX && cy == s->startY)) {
        push(&s->path, ((Cell){cx, cy}));
        int p = s->parent[cy * s->N + cx];
        if (p < 0) break;
        cx = p % s->N;
        cy = p / s->N;
    }
    push(&s->path, ((Cell){s->startX, s->startY}));

    for (size_t i = 0; i < s->path.len / 2; i++) {
        Cell tmp = s->path.data[i];
        s->path.data[i] = s->path.data[s->path.len - 1 - i];
        s->path.data[s->path.len - 1 - i] = tmp;
    }
    return (int)s->path.len;
}

static inline void search_free(SearchState *s) {
    free(s->visited);
    free(s->parent);
    free(s->dist);
    free(s->processed);
    free(s->fscore);
    free(s->heap);
    free(s->heap_pos);
    release(&s->queue);
    release(&s->path);
}

// -------- Min-heap helpers (uses external score array) --------
static inline void heap_swap(SearchState *s, int a, int b) {
    int va = s->heap[a];
    int vb = s->heap[b];
    s->heap[a] = vb;
    s->heap[b] = va;
    s->heap_pos[va] = b;
    s->heap_pos[vb] = a;
}

static inline void heap_up(SearchState *s, const int *score, int idx) {
    while (idx > 0) {
        int p = (idx - 1) / 2;
        if (score[s->heap[idx]] >= score[s->heap[p]]) break;
        heap_swap(s, idx, p);
        idx = p;
    }
}

static inline void heap_down(SearchState *s, const int *score, int idx) {
    for (;;) {
        int l = idx * 2 + 1;
        int r = idx * 2 + 2;
        int smallest = idx;
        if (l < s->heap_len && score[s->heap[l]] < score[s->heap[smallest]]) smallest = l;
        if (r < s->heap_len && score[s->heap[r]] < score[s->heap[smallest]]) smallest = r;
        if (smallest == idx) break;
        heap_swap(s, idx, smallest);
        idx = smallest;
    }
}

static inline void heap_push(SearchState *s, const int *score, int v) {
    s->heap[s->heap_len] = v;
    s->heap_pos[v] = s->heap_len;
    heap_up(s, score, s->heap_len);
    s->heap_len++;
}

static inline int heap_pop(SearchState *s, const int *score) {
    int v = s->heap[0];
    s->heap_pos[v] = -1;
    s->heap_len--;
    if (s->heap_len > 0) {
        s->heap[0] = s->heap[s->heap_len];
        s->heap_pos[s->heap[0]] = 0;
        heap_down(s, score, 0);
    }
    return v;
}

static inline void heap_decrease_key(SearchState *s, const int *score, int v) {
    int idx = s->heap_pos[v];
    if (idx >= 0) heap_up(s, score, idx);
}
