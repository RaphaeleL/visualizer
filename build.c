#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "libs/build.h"

int main() {
    auto_rebuild(__FILE__);

    Cmd cmd_plasma = {0};
    push(&cmd_plasma, "cc");
    push(&cmd_plasma, "-O3");
    push(&cmd_plasma, "-march=native");
    push(&cmd_plasma, "-ffast-math");
    push(&cmd_plasma, "-funroll-loops");
    push(&cmd_plasma, "-Wall");
    push(&cmd_plasma, "-Wextra");
    push(&cmd_plasma, "-I./libs/raylib-5.5_macos/include");
    push(&cmd_plasma, "-L./libs/raylib-5.5_macos/lib");
    push(&cmd_plasma, "-Wl,-rpath,@executable_path/libs/raylib-5.5_macos/lib");
    push(&cmd_plasma, "-lraylib");
    push(&cmd_plasma, "-lm");
    push(&cmd_plasma, "-o", "main", "main.c");

    if (!run_always(&cmd_plasma)) return 1;

    return 0;
}
