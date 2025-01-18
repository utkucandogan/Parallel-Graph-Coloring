# Parallel Graph Coloring
Parallel graph coloring using MPI

## How to Compile
Use `make` to compile the relase mode with parallel method.

## how to Run
- The first argument of the program is either 'broadcaster' or 'scheduler' which uses either built-in broadcast or a custom scheduling method.
- The second argument of the program is the filepath for a matrix.

Example call `mpirun -np 8 ./pgc_parallel.elf broadcaster data/matrix_39x39.bin`
