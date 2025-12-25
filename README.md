# Visualizer

Tiny raylib playground that renders a random maze and visualizes search algorithms, a simple sorting visualizer or even an interactive function plotter.

![Maze](assets/example_maze.png)

## Requirements

- C compiler 
- [raylib](https://github.com/raysan5/raylib) - Bundled `libs/raylib-5.5_macos/` (no extra install needed)
- [build.h](https://github.com/RaphaeleL/build.h) - Bundled `libs/build.h` (no extra install needed)

## Build

- Compile the builder: `cc build.c -o build`
- Compile the app: `./build`

The helper emits a binary named `main`.

## Run

- Maze search visualizer: `./main maze`
- Sorting visualizer: `./main sort`
- Function visualizer: `./main plotter <function>` (e.g. `sin(x) + x^2`, etc.)
- Help: `./main usage`

### Controls

- `Esc` to quit
- `r` to re-generate the maze or reset the sorter, do nothing in the plotter

## Switching algorithms

- **Maze search**: edit `maze.h` and swap which header is included under the “Choose one algorithm” section (bfs/dfs/greedy/astar/dijkstra).
- **Sorting**: edit `sort.h` and choose one include under its “Choose one algorithm” section (bubble/selection/merge/quick/heap).
- **Function plotter**: set the function as a command line argument when running `./main plotter <function>` or edit `plotter.h` and change the default function `static double f(double x)`.

## Further Example

### Interactive Function Plotter 

![Plotter](assets/example_plotter.png)

### Display of Sorting Algorithm

![Sort](assets/example_sort.png)

### Maze Search Visualization

![Maze](assets/example_maze.png)

