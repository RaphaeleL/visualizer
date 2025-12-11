#pragma once
#include "common.h"

#define ALGO_NAME "A*"

// A* step
static inline bool step(SearchState *s) {
    int bestIdx = -1;
    int bestScore = INF;
    for (int i = 0; i < s->max; i++) {
        if (s->visited[i] && !s->processed[i]) {
            int cx = i % s->N;
            int cy = i / s->N;
            int h = abs(cx - s->goalX) + abs(cy - s->goalY);
            int score = s->dist[i] + h;
            if (score < bestScore) {
                bestScore = score;
                bestIdx = i;
            }
        }
    }
    if (bestIdx == -1) return false;

    int x = bestIdx % s->N;
    int y = bestIdx / s->N;
    s->processed[bestIdx] = 1;

    if (x == s->goalX && y == s->goalY) {
        build_path(s);
        return true;
    }

    for (int i = 0; i < 4; i++) {
        int nx = x + dirs[i][0];
        int ny = y + dirs[i][1];
        if (nx >= 0 && nx < s->N && ny >= 0 && ny < s->N && s->maze[ny][nx] == PATH) {
            int idx = ny * s->N + nx;
            int nd = s->dist[y * s->N + x] + 1;
            if (nd < s->dist[idx]) {
                s->dist[idx] = nd;
                s->parent[idx] = y * s->N + x;
                s->visited[idx] = 1;
            }
        }
    }
    return false;
}
