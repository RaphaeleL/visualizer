#define QOL_IMPLEMENTATION
#include "libs/build.h"

#include "maze.h"
#include "sort.h"

void usage() {
    qol_warn("Usage: <program> <param>\n");
    qol_warn("param:\n");
    qol_warn("  maze   - Path finding Algorithms like Dijkstra.\n");
    qol_warn("  sort   - Sorting Algorithms like Merge Sort.\n");
    qol_warn("  usage  - Show this usage information\n");
}

int main(int argc, char** argv) {
    const char* program = qol_shift(argc, argv); 
    if (argc < 1) qol_error("Usage: %s <type>\n", program);

    const char* val = qol_shift(argc, argv);

    if (strcmp(val, "maze") == 0) maze();
    if (strcmp(val, "sort") == 0) sort();
    if (strcmp(val, "usage") == 0) usage();
    else qol_error("Unknown type: %s\n", val);


    return 0;
}
