#include <stdlib.h>
#define QOL_IMPLEMENTATION
#define QOL_STRIP_PREFIX
#include "libs/build.h"

void base(Cmd* command) {
    push(command, "cc");
    push(command, "-O3");
    push(command, "-Wall");
    push(command, "-Wextra");
    push(command, "-Wno-unused-function");
    push(command, "-I./libs/raylib-5.5_macos/include");
}

Cmd cmd = {0};
Procs procs = {0};

int main() {
    auto_rebuild(__FILE__);

    const char* src_folders[] = {"entries", "algorithms/maze", "algorithms/sort"};
    const char* out_folder = "out";
    list(const char*) files = {0};

    mkdir_if_not_exists(out_folder);

    // Compile the entries/*.c and algorithms/*/*.c source files
    for (size_t sf = 0; sf < ARRAY_LEN(src_folders); sf++) {
        const char* src_folder = src_folders[sf];
        String contents = {0};

        if (!read_dir(src_folder, &contents)) return EXIT_FAILURE;

        for (size_t i = 0; i < contents.len; i++) {
            const char* src_path = contents.data[i];
            
            if (!str_ends_with(src_path, ".c")) continue;
            const char* temp = str_replace(src_path, ".c", ".o");
            const char* new_path = str_replace(temp, src_folder, out_folder);
            if (!temp || !new_path) continue;

            base(&cmd);
            push(&cmd, "-c", src_path, "-o", new_path);
            if (!run(&cmd)) return EXIT_SUCCESS;
            push(&files, new_path);
        }
    }

    // Compile the main.c
    base(&cmd);
    push(&cmd, "-c", "main.c", "-o", "out/main.o");
    if (!run(&cmd)) return EXIT_SUCCESS;
    push(&files, "out/main.o");

    // Link all object files into the final executable
    Cmd link = (Cmd){0};
    base(&link);
    push(&link, "-L./libs/raylib-5.5_macos/lib");
    push(&link, "-Wl,-rpath,@executable_path/libs/raylib-5.5_macos/lib");
    push(&link, "-lraylib", "-lm");
    push(&link, "-o", "main");
    for (size_t i = 0; i < files.len; i++) push(&link, files.data[i]);
    
    if (!run_always(&link)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
