#include <stdlib.h>
#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "libs/build.h"

int main() {
    auto_rebuild(__FILE__);

    const char *files[][2] = {
        { "main.c",                     "out/main.o" },
        { "entries/maze.c",             "out/maze.o" },
        { "entries/sort.c",             "out/sort.o" },
        { "entries/plotter.c",          "out/plotter.o" },
        { "entries/hexdump.c",          "out/hexdump.o" },
        { "algorithms/maze/common.c",   "out/common.o" },
        { "algorithms/maze/bfs.c",      "out/bfs.o" },
        { "algorithms/maze/dfs.c",      "out/dfs.o" },
        { "algorithms/maze/dijkstra.c", "out/dijkstra.o" },
        { "algorithms/maze/astar.c",    "out/astar.o" },
        { "algorithms/maze/greedy.c",   "out/greedy.o" },
        { "algorithms/sort/heap.c",     "out/heap.o" },
        { "algorithms/sort/merge.c",    "out/merge.o" },
        { "algorithms/sort/quick.c",    "out/quick.o" },
        { "algorithms/sort/selection.c","out/selection.o" },
        { "algorithms/sort/bubble.c",   "out/bubble.o" },
    };

    mkdir_if_not_exists("out/");

    for (size_t i = 0; i < ARRAY_LEN(files); i++) {
        Cmd cc = (Cmd){0};
        push(&cc, "cc");
        push(&cc, "-O3");
        push(&cc, "-Wall");
        push(&cc, "-Wextra");
        push(&cc, "-Wno-unused-function");
        push(&cc, "-I./libs/raylib-5.5_macos/include");
        push(&cc, "-c");
        push(&cc, files[i][0]);
        push(&cc, "-o");
        push(&cc, files[i][1]);
        if (!run_always(&cc)) return EXIT_FAILURE;
    }

    Cmd link = (Cmd){0};
    push(&link, "cc");
    push(&link, "-O3");
    push(&link, "-Wall");
    push(&link, "-Wextra");
    push(&link, "-I./libs/raylib-5.5_macos/include");
    push(&link, "-L./libs/raylib-5.5_macos/lib");
    push(&link, "-Wl,-rpath,@executable_path/libs/raylib-5.5_macos/lib");
    push(&link, "-lraylib");
    push(&link, "-lm");
    push(&link, "-o");
    push(&link, "main");
    for (size_t i = 0; i < ARRAY_LEN(files); i++) {
        push(&link, files[i][1]);
    }

    if (!run_always(&link)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
