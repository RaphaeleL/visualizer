// Bottom-up Merge Sort (single-step)
#pragma once

#define SORT_ALGO_NAME "Merge Sort"

static inline void sort_init(SortState *s) {
    s->mergeWidth = 1;
    s->mergeLeft = 0;
    s->mergeMid = 0;
    s->mergeRight = 0;
    s->mergeK = 0;
    s->mergeCopying = false;
}

static inline bool sort_step(SortState *s) {
    if (s->finished || s->n <= 1) {
        s->finished = true;
        s->timeDone = qol_timer_elapsed(&s->timer);
        return true;
    }

    int n = s->n;
    if (s->mergeLeft >= n) {
        s->mergeWidth *= 2;
        s->mergeLeft = 0;
    }

    if (s->mergeWidth >= n) {
        s->finished = true;
        s->timeDone = qol_timer_elapsed(&s->timer);
        return true;
    }

    int left = s->mergeLeft;
    int mid = left + s->mergeWidth;
    int right = left + 2 * s->mergeWidth;
    if (mid > n) mid = n;
    if (right > n) right = n;

    if (!s->mergeCopying && s->mergeK == 0) {
        s->i = left;
        s->j = mid;
        s->mergeK = left;
    }

    if (!s->mergeCopying) {
        if (s->i < mid && s->j < right) {
            s->highlightA = s->i;
            s->highlightB = s->j;
            s->comparisons++;
            if (s->values[s->i] <= s->values[s->j]) {
                s->aux[s->mergeK++] = s->values[s->i++];
            } else {
                s->aux[s->mergeK++] = s->values[s->j++];
            }
        } else if (s->i < mid) {
            s->aux[s->mergeK++] = s->values[s->i++];
        } else if (s->j < right) {
            s->aux[s->mergeK++] = s->values[s->j++];
        }

        if (s->i >= mid && s->j >= right) {
            s->mergeCopying = true;
            s->mergeK = left;
        }
    } else {
        // copy back one item per step
        s->highlightA = s->mergeK;
        s->values[s->mergeK] = s->aux[s->mergeK];
        s->mergeK++;
        if (s->mergeK >= right) {
            s->mergeLeft = right;
            s->mergeK = 0;
            s->mergeCopying = false;
        }
    }

    return s->finished;
}
