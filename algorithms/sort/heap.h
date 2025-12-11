// Heap Sort (single-step)
#pragma once

#define SORT_ALGO_NAME "Heap Sort"

static inline void sort_init(SortState *s) {
    s->heapSize = s->n;
    s->heapBuildIdx = (s->n / 2) - 1;
    s->heapPhase = 0; // 0 build, 1 sort
}

static inline void heap_sift_down(SortState *s, int idx) {
    while (1) {
        int left = 2 * idx + 1;
        int right = left + 1;
        int largest = idx;
        if (left < s->heapSize) {
            s->comparisons++;
            if (s->values[left] > s->values[largest]) largest = left;
        }
        if (right < s->heapSize) {
            s->comparisons++;
            if (s->values[right] > s->values[largest]) largest = right;
        }
        if (largest != idx) {
            int tmp = s->values[idx];
            s->values[idx] = s->values[largest];
            s->values[largest] = tmp;
            s->swaps++;
            idx = largest;
        } else {
            break;
        }
    }
}

static inline bool sort_step(SortState *s) {
    if (s->finished || s->n <= 1) {
        s->finished = true;
        s->timeDone = qol_timer_elapsed(&s->timer);
        return true;
    }

    if (s->heapPhase == 0) {
        if (s->heapBuildIdx < 0) {
            s->heapPhase = 1;
        } else {
            s->highlightA = s->heapBuildIdx;
            heap_sift_down(s, s->heapBuildIdx);
            s->heapBuildIdx--;
            return false;
        }
    }

    if (s->heapSize <= 1) {
        s->finished = true;
        s->timeDone = qol_timer_elapsed(&s->timer);
        return true;
    }

    s->highlightA = 0;
    s->highlightB = s->heapSize - 1;
    int tmp = s->values[0];
    s->values[0] = s->values[s->heapSize - 1];
    s->values[s->heapSize - 1] = tmp;
    s->swaps++;
    s->heapSize--;
    heap_sift_down(s, 0);

    return false;
}
