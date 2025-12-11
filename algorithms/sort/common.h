#pragma once

typedef struct SortState {
    int *values;
    int *aux;
    int *stackL;
    int *stackR;
    int n;
    int i;
    int j;
    int k;
    int minIdx;
    int comparisons;
    int swaps;
    bool finished;
    bool swappedLast;
    int highlightA;
    int highlightB;
    double timeDone;
    QOL_Timer timer;

    // merge
    int mergeWidth;
    int mergeLeft;
    int mergeMid;
    int mergeRight;
    int mergeK;
    bool mergeCopying;

    // quick
    int quickLeft;
    int quickRight;
    int quickPivot;
    int quickI;
    int quickJ;
    bool quickPartitioning;
    int stackTop;

    // heap
    int heapSize;
    int heapBuildIdx;
    int heapPhase;
} SortState;
