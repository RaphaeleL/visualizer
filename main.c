#define QOL_IMPLEMENTATION
#include "libs/build.h"

#include "maze.h"
#include "sort.h"
#include "plotter.h"

typedef void (*cmd_fn)(const char *arg);
typedef struct {
    const char *name;
    cmd_fn fn;
} Command;

void usage_wrapper(const char *arg) {
    (void)arg; // unused
    qol_warn("Usage: <program> <param> [args...]\n");
    qol_warn("param:\n");
    qol_warn("  maze    - Path finding Algorithms like Dijkstra.\n");
    qol_warn("  sort    - Sorting Algorithms like Merge Sort.\n");
    qol_warn("  plotter [function] - Function Plotter for f(x).\n");
    qol_warn("           Example: ./main plotter \"sin(x) * cos(x)\"\n");
    qol_warn("           Example: ./main plotter \"x^3 - 2*x + 1\"\n");
    qol_warn("  usage   - Show this usage information\n");
}

void maze_wrapper(const char *arg) {
    (void)arg; // unused
    maze();
}

void sort_wrapper(const char *arg) {
    (void)arg; // unused
    sort();
}

static Command commands[] = {
    { "maze",    maze_wrapper },
    { "sort",    sort_wrapper },
    { "plotter", plotter_wrapper },
    { "usage",   usage_wrapper },
};


cmd_fn lookup_command(const char *name) {
    for (int i = 0; i < (int)QOL_ARRAY_LEN(commands); i++) {
        if (strcmp(commands[i].name, name) == 0) {
            return commands[i].fn;
        }
    }
    return NULL;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        usage_wrapper(NULL);
        return EXIT_FAILURE;
    }

    qol_shift(argc, argv); 
    const char* val = qol_shift(argc, argv);
    cmd_fn fn = lookup_command(val);

    if (!fn) {
        qol_error("Unknown type: %s\n", val);
        usage_wrapper(NULL);
        return EXIT_FAILURE;
    }

    // Get optional argument (for plotter function string)
    const char* arg = (argc > 0) ? qol_shift(argc, argv) : NULL;
    fn(arg);

    return EXIT_SUCCESS;
}
