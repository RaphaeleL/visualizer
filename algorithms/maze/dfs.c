#include "../../libs/build.h"
#include "common.h"
#include "dfs.h"

// Depth-first search step
bool dfs_step(SearchState *s) {
    if (s->queue.len == 0) return false;
    Cell c = s->queue.data[s->queue.len - 1];
    qol_drop(&s->queue);
    int x = c.x, y = c.y;

    if (x == s->goalX && y == s->goalY) {
        build_path(s);
        return true;
    }

    for (int i = 0; i < 4; i++) {
        int nx = x + dirs[i][0];
        int ny = y + dirs[i][1];
        if (nx >= 0 && nx < s->N && ny >= 0 && ny < s->N && s->maze[ny][nx] == PATH) {
            int idx = ny * s->N + nx;
            if (!s->visited[idx]) {
                s->visited[idx] = 1;
                s->parent[idx] = y * s->N + x;
                qol_push(&s->queue, ((Cell){nx, ny}));
            }
        }
    }
    return false;
}
