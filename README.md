# Parallel Graph Coloring
Parallel graph coloring using MPI

## Implementation Details
- `Adjacency`: Holds the adjacency list for every vertex. Vertices are zero-indexed.
    - `Adjacency::operator[]`: This operator is used for obtaining individual adjacency list. It returns `const index_t*` which can be iterated until it points to `Adjacency::END` which indicates the end of the list.
- `ColorArray`: List of colors for every vertex. Vertices are zero-indexed. Colors are one-indexed. Color zero is reserved for uncolored vertices.
    - `ColorArray::operator[]`: can be used for obtaining the color of a vertex.
- `ForbiddenColors`: A class to hold forbidden colors. It should initialized with color count `ColorArray::max_color()`. Internally, it holds an array of size `ColorArray::max_color() + 1` which is used for reserving space for color zero. This eases the indexing of the array.

### Standard Way of Iterating over Adjacencies
```cpp
auto adj = Adjacency::create("data/sudoku.bin");
for (Adjacency::index_t vertex = 0 ; vertex < adj.count() ; ++vertex) {
    std::cout << "Vertex " << vertex << ": ";
    for (auto neighbor = adj[vertex] ; *neighbor != Adjacency::END ; ++neighbor) {
        std::cout << *neighbor << ' ';
    }
    std::cout << "\n";
}
```

## Debugging
For debugging compile by using `make debug` to enable debug symbols and a `DEBUG` macro which enables the use of the `log(const char* fmt, ...)` function.

- `log(const char* fmt, ...)`: Prints only when compiled with `make debug`. Follows the same syntax as `printf`.

> When changing from release to debug or vice-versa, always use `make clean` to ensure rebuilding of all files.
