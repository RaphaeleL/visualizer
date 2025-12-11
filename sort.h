#pragma once
#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#define QOL_IMPLEMENTATION
#include "libs/build.h"

#define SORT_TICK 0.0f
#define SORT_MAX_VALUE 420

#include "algorithms/sort/common.h"

// Choose one algorithm (comment/uncomment a single include):
// #include "algorithms/sort/bubble.h"
// #include "algorithms/sort/selection.h"
#include "algorithms/sort/merge.h"
// #include "algorithms/sort/quick.h"
// #include "algorithms/sort/heap.h"

static void sort_state_reset_common(SortState *s, int n) {
    s->n = n;
    s->i = s->j = s->k = 0;
    s->minIdx = 0;
    s->comparisons = 0;
    s->swaps = 0;
    s->finished = false;
    s->swappedLast = false;
    s->highlightA = s->highlightB = -1;
    s->timeDone = 0.0;
    s->mergeWidth = s->mergeLeft = s->mergeMid = s->mergeRight = s->mergeK = 0;
    s->mergeCopying = false;
    s->quickPartitioning = false;
    s->stackTop = -1;
    s->heapPhase = 0;
    s->heapBuildIdx = (n / 2) - 1;
    s->heapSize = n;
    if (!s->values) s->values = malloc(sizeof(int) * n);
    if (!s->aux) s->aux = malloc(sizeof(int) * n);
    if (!s->stackL) s->stackL = malloc(sizeof(int) * n);
    if (!s->stackR) s->stackR = malloc(sizeof(int) * n);
    for (int idx = 0; idx < n; idx++) s->values[idx] = 10 + rand() % SORT_MAX_VALUE;
    qol_timer_start(&s->timer);
}

static void draw_bars(const SortState *s, int screenW, int screenH) {
    const int margin = 40;
    float barWidth = (float)(screenW - 2 * margin) / (float)s->n;
    for (int idx = 0; idx < s->n; idx++) {
        int h = s->values[idx];
        int x = (int)(margin + idx * barWidth);
        int y = screenH - h - margin;

        Color col = RAYWHITE;
        if (s->finished) col = GREEN;
        if (idx == s->highlightA || idx == s->highlightB) col = s->swappedLast ? RED : YELLOW;
        DrawRectangle(x, y, (int)(barWidth - 1), h, col);
    }
}

void sort(void) {
    const int SCREEN_W = 1000;
    const int SCREEN_H = 720;
    const int N = 120;

    unsigned int seed_value = (unsigned int)time(NULL);
    srand(seed_value);

    SortState state = {0};
    sort_state_reset_common(&state, N);
    sort_init(&state);

    float tickTime = 0.0f;

    InitWindow(SCREEN_W, SCREEN_H, TextFormat("Sorting Visualizer - %s", SORT_ALGO_NAME));
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            if (IsKeyPressed(KEY_R)) {
                sort_state_reset_common(&state, N);
                sort_init(&state);
            }

            ClearBackground(BLACK);

            float frameTime = GetFrameTime();
            tickTime += frameTime;
            if (!state.finished && tickTime >= SORT_TICK) {
                tickTime = 0.0f;
                sort_step(&state);
                if (state.finished) state.timeDone = qol_timer_elapsed(&state.timer);
            }

            draw_bars(&state, SCREEN_W, SCREEN_H);

            // HUD
            const int panelW = 340;
            const int panelH = 170;
            const int panelX = 20;
            const int panelY = 20;
            DrawRectangle(panelX, panelY, panelW, panelH, Fade(BLACK, 0.7f));
            DrawRectangleLines(panelX, panelY, panelW, panelH, RAYWHITE);

            char buf[128];
            int lineY = panelY + 12;
            DrawText("Sorting (press r to rerun)", panelX + 10, lineY, 20, RAYWHITE); lineY += 24;

            snprintf(buf, sizeof(buf), "algo: %s", SORT_ALGO_NAME);
            DrawText(buf, panelX + 10, lineY, 18, YELLOW); lineY += 20;

            snprintf(buf, sizeof(buf), "time: %.3fs", state.finished ? state.timeDone : qol_timer_elapsed(&state.timer));
            DrawText(buf, panelX + 10, lineY, 18, YELLOW); lineY += 20;

            snprintf(buf, sizeof(buf), "tick: %.3fs", (double)SORT_TICK);
            DrawText(buf, panelX + 10, lineY, 18, YELLOW); lineY += 20;

            snprintf(buf, sizeof(buf), "comparisons: %d", state.comparisons);
            DrawText(buf, panelX + 10, lineY, 18, YELLOW); lineY += 20;

            snprintf(buf, sizeof(buf), "swaps: %d", state.swaps);
            DrawText(buf, panelX + 10, lineY, 18, YELLOW); lineY += 20;

            DrawText("Esc to quit", panelX + 10, lineY, 16, GRAY);
        EndDrawing();
    }

    CloseWindow();
    free(state.values);
    free(state.aux);
    free(state.stackL);
    free(state.stackR);
}
