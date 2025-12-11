#pragma once
#include "common.h"

#define ALGO_NAME "Dijkstra"

static inline void heap_swap(SearchState *s, int a, int b) {
    int va = s->heap[a];
    int vb = s->heap[b];
    s->heap[a] = vb;
    s->heap[b] = va;
    s->heap_pos[va] = b;
    s->heap_pos[vb] = a;
}

static inline void heap_up(SearchState *s, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (s->dist[s->heap[idx]] >= s->dist[s->heap[parent]]) break;
        heap_swap(s, idx, parent);
        idx = parent;
    }
}

static inline void heap_down(SearchState *s, int idx) {
    while (true) {
        int left = 2 * idx + 1;
        int right = left + 1;
        int smallest = idx;
        if (left < s->heap_len && s->dist[s->heap[left]] < s->dist[s->heap[smallest]]) {
            smallest = left;
        }
        if (right < s->heap_len && s->dist[s->heap[right]] < s->dist[s->heap[smallest]]) {
            smallest = right;
        }
        if (smallest == idx) break;
        heap_swap(s, idx, smallest);
        idx = smallest;
    }
}

static inline void heap_push(SearchState *s, int cell) {
    s->heap[s->heap_len] = cell;
    s->heap_pos[cell] = s->heap_len;
    heap_up(s, s->heap_len);
    s->heap_len++;
}

static inline int heap_pop(SearchState *s) {
    if (s->heap_len == 0) return -1;
    int top = s->heap[0];
    s->heap_len--;
    if (s->heap_len > 0) {
        s->heap[0] = s->heap[s->heap_len];
        s->heap_pos[s->heap[0]] = 0;
        heap_down(s, 0);
    }
    s->heap_pos[top] = -1;
    return top;
}

static inline void heap_decrease_key(SearchState *s, int cell) {
    int pos = s->heap_pos[cell];
    if (pos >= 0) heap_up(s, pos);
    else heap_push(s, cell);
}

// Dijkstra step (using min-heap)
static inline bool step(SearchState *s) {
    if (s->heap_len == 0) {
        int startIdx = s->startY * s->N + s->startX;
        heap_push(s, startIdx);
    }

    int bestIdx = heap_pop(s);
    if (bestIdx == -1) return false;
    if (s->processed[bestIdx]) return false;

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
            if (s->processed[idx]) continue;
            int nd = s->dist[y * s->N + x] + 1;
            if (nd < s->dist[idx]) {
                s->dist[idx] = nd;
                s->parent[idx] = y * s->N + x;
                s->visited[idx] = 1;
                heap_decrease_key(s, idx);
            }
        }
    }
    return false;
}
