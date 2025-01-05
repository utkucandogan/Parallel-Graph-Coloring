This is the initial sequential algorithm with MPI scatter and all gather thrown in to work with multiple processes.

It will create a random adjacency array with given vertex count and size than scatter it evenly (process count must be a even divisor of  the vertex count)
then each process will color seperately after which all gathering the colors and ending. Currently does not handle collisions at all and can be run using only 1 process
which makes it effectively the sequential algorithm.
