#define QOL_IMPLEMENTATION
#include "libs/build.h"

#include "maze.h"
#include "sort.h"

typedef void (*cmd_fn)(void);
typedef struct {
    const char *name;
    cmd_fn fn;
} Command;

void usage() {
    qol_warn("Usage: <program> <param>\n");
    qol_warn("param:\n");
    qol_warn("  maze   - Path finding Algorithms like Dijkstra.\n");
    qol_warn("  sort   - Sorting Algorithms like Merge Sort.\n");
    qol_warn("  usage  - Show this usage information\n");
}

static Command commands[] = {
    { "maze",  maze },
    { "sort",  sort },
    { "usage", usage },
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
    if (argc < 2) usage();
    

    qol_shift(argc, argv); 
    const char* val = qol_shift(argc, argv);
    cmd_fn fn = lookup_command(val);

    if (!fn) {
        qol_error("Unknown type: %s\n", val);
        usage();
        return EXIT_FAILURE;
    }

    fn();

    return EXIT_SUCCESS;
}
