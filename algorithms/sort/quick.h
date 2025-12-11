// Quick Sort (single-step, Lomuto partition with stack)
#pragma once

#define SORT_ALGO_NAME "Quick Sort"

static inline void sort_init(SortState *s) {
    s->stackTop = -1;
    // push initial range
    s->stackTop++;
    s->stackL[s->stackTop] = 0;
    s->stackR[s->stackTop] = s->n - 1;
    s->quickPartitioning = false;
}

static inline bool sort_step(SortState *s) {
    if (s->finished || s->n <= 1) {
        s->finished = true;
        s->timeDone = qol_timer_elapsed(&s->timer);
        return true;
    }

    if (!s->quickPartitioning) {
        if (s->stackTop < 0) {
            s->finished = true;
            s->timeDone = qol_timer_elapsed(&s->timer);
            return true;
        }
        s->quickLeft = s->stackL[s->stackTop];
        s->quickRight = s->stackR[s->stackTop];
        s->stackTop--;
        s->quickPivot = s->values[s->quickRight];
        s->quickI = s->quickLeft - 1;
        s->quickJ = s->quickLeft;
        s->quickPartitioning = true;
    }

    if (s->quickJ < s->quickRight) {
        s->highlightA = s->quickJ;
        s->highlightB = s->quickRight;
        s->comparisons++;
        if (s->values[s->quickJ] <= s->quickPivot) {
            s->quickI++;
            if (s->quickI != s->quickJ) {
                int tmp = s->values[s->quickI];
                s->values[s->quickI] = s->values[s->quickJ];
                s->values[s->quickJ] = tmp;
                s->swaps++;
            }
        }
        s->quickJ++;
        return false;
    } else {
        int pivotPos = s->quickI + 1;
        if (pivotPos != s->quickRight) {
            int tmp = s->values[pivotPos];
            s->values[pivotPos] = s->values[s->quickRight];
            s->values[s->quickRight] = tmp;
            s->swaps++;
        }
        // push subranges
        int leftLen = pivotPos - 1 - s->quickLeft;
        int rightLen = s->quickRight - (pivotPos + 1);
        if (leftLen > 0) {
            s->stackTop++;
            s->stackL[s->stackTop] = s->quickLeft;
            s->stackR[s->stackTop] = pivotPos - 1;
        }
        if (rightLen > 0) {
            s->stackTop++;
            s->stackL[s->stackTop] = pivotPos + 1;
            s->stackR[s->stackTop] = s->quickRight;
        }
        s->quickPartitioning = false;
        return false;
    }
}
