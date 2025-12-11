// Selection Sort (single-step)
#pragma once

#define SORT_ALGO_NAME "Selection Sort"

static inline void sort_init(SortState *s) {
    s->i = 0;
    s->j = 1;
    s->minIdx = 0;
}

static inline bool sort_step(SortState *s) {
    if (s->finished || s->n <= 1) {
        s->finished = true;
        s->timeDone = qol_timer_elapsed(&s->timer);
        return true;
    }

    if (s->i >= s->n - 1) {
        s->finished = true;
        s->timeDone = qol_timer_elapsed(&s->timer);
        return true;
    }

    s->highlightA = s->i;
    s->highlightB = s->j;

    s->comparisons++;
    if (s->values[s->j] < s->values[s->minIdx]) {
        s->minIdx = s->j;
    }

    s->j++;
    if (s->j >= s->n) {
        if (s->minIdx != s->i) {
            int tmp = s->values[s->i];
            s->values[s->i] = s->values[s->minIdx];
            s->values[s->minIdx] = tmp;
            s->swaps++;
        }
        s->i++;
        s->minIdx = s->i;
        s->j = s->i + 1;
    }
    return s->finished;
}
