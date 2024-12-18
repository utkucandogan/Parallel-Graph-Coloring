#include <iostream>
#include "matrix.hpp"

int main()
{
    std::cout << "Hello world" << std::endl;

    auto adj = Adjacency::create("data/sudoku.dat");
    for (size_t i = 0 ; i < adj.count() ; ++i) {
        for (auto n : adj[i].to_vector()) {
            std::cout << n << ' ';
        }
        std::cout << "\n";
    }
}
