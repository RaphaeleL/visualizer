// Bubble Sort (single-step)
#pragma once

#define SORT_ALGO_NAME "Bubble Sort"

static inline void sort_init(SortState *s) {
    (void)s;
}

// Returns true once the array is sorted
static inline bool sort_step(SortState *s) {
    if (s->finished || s->n <= 1) {
        s->finished = true;
        s->timeDone = qol_timer_elapsed(&s->timer);
        return true;
    }

    int limit = s->n - 1 - s->i;
    if (limit <= 0) {
        s->finished = true;
        s->timeDone = qol_timer_elapsed(&s->timer);
        return true;
    }

    s->highlightA = s->j;
    s->highlightB = s->j + 1;
    s->comparisons++;
    s->swappedLast = false;

    if (s->values[s->j] > s->values[s->j + 1]) {
        int tmp = s->values[s->j];
        s->values[s->j] = s->values[s->j + 1];
        s->values[s->j + 1] = tmp;
        s->swaps++;
        s->swappedLast = true;
    }

    s->j++;
    if (s->j >= limit) {
        s->j = 0;
        s->i++;
        if (s->i >= s->n - 1) {
            s->finished = true;
            s->timeDone = qol_timer_elapsed(&s->timer);
            return true;
        }
    }
    return s->finished;
}
