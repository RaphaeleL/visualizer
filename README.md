# Visualizer

Tiny raylib playground that renders several visualizations for algorithms or functions.

![Maze](assets/example_maze.png)

## Requirements

- C compiler 
- [raylib](https://github.com/raysan5/raylib) - Bundled `libs/raylib-5.5_macos/` (no extra install needed)
- [build.h](https://github.com/RaphaeleL/build.h) - Bundled `libs/build.h` (no extra install needed)

## Usage 

First you have to build the application:

```bash
cc build.c -o build` # Compile the builder
./build # Compile the app
```

The helper emits a binary named `main`. Now you can run the visualizer itself:

```bash
./main maze # Maze search visualizer
./main sort # Sorting visualizer
./main plotter "x^2" # Function visualizer
./main usage # Help
```

Since its a raylib application, it opens a window to display the visualizations, press `Esc` to quit. Inside the maze or the sorter, you can press `r` to reset/regenerate.

To switch between between alogorithms or functions, use the `entries/*.h` Header Files.

- **Maze search**: edit `entries/maze.h` and swap which header is included under the “Choose one algorithm” section (bfs/dfs/greedy/astar/dijkstra).
- **Sorting**: edit `entries/sort.h` and choose one include under its “Choose one algorithm” section (bubble/selection/merge/quick/heap).
- **Function plotter**: set the function as a command line argument when running `./main plotter <function>` or edit `entries/plotter.h` and change the default function `static double f(double x)`.

## Further Example

### Interactive Function Plotter 

![Plotter](assets/example_plotter.png)

### Display of Sorting Algorithm

![Sort](assets/example_sort.png)

### Maze Search Visualization

![Maze](assets/example_maze.png)

