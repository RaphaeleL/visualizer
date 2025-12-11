#define QOL_IMPLEMENTATION
#include "libs/build.h"

#include "maze.h"
#include "sort.h"

void usage() {
    qol_warn("Usage: <program> <param>\n");
    qol_warn("param:\n");
    qol_warn("  maze   - Generate and display a maze\n");
    qol_warn("  sort   - Generate and display a sort\n");
    qol_warn("  usage  - Show this usage information\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        qol_error("Usage: {} <type>\n", argv[0]);
        return 1;
    }

    qol_shift(argc, argv); 
    const char* val = qol_shift(argc, argv);

    if (strcmp(val, "maze") == 0) maze();
    if (strcmp(val, "sort") == 0) sort();
    if (strcmp(val, "usage") == 0) usage();
    else qol_warn("Unknown type: {}\n", val);


    return 0;
}
