#include "../../libs/build.h"
#include "common.h"

// 4-neighborhood directions (up, right, down, left).
int dirs[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

void search_init(SearchState *s, int N, int **maze, int sx, int sy, int gx, int gy) {
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

    qol_push(&s->queue, ((Cell){sx, sy}));
    s->head = 0;
    s->visited[sy * N + sx] = 1;
    s->dist[sy * N + sx] = 0;
}

int build_path(SearchState *s) {
    s->path.len = 0;
    int cx = s->goalX;
    int cy = s->goalY;

    while (!(cx == s->startX && cy == s->startY)) {
        qol_push(&s->path, ((Cell){cx, cy}));
        int p = s->parent[cy * s->N + cx];
        if (p < 0) break;
        cx = p % s->N;
        cy = p / s->N;
    }
    qol_push(&s->path, ((Cell){s->startX, s->startY}));

    for (size_t i = 0; i < s->path.len / 2; i++) {
        Cell tmp = s->path.data[i];
        s->path.data[i] = s->path.data[s->path.len - 1 - i];
        s->path.data[s->path.len - 1 - i] = tmp;
    }
    return (int)s->path.len;
}

void search_free(SearchState *s) {
    free(s->visited);
    free(s->parent);
    free(s->dist);
    free(s->processed);
    free(s->fscore);
    free(s->heap);
    free(s->heap_pos);
    qol_release(&s->queue);
    qol_release(&s->path);
}
