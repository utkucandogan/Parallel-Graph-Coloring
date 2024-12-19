#include <iostream>
#include "matrix.hpp"

int main(int argc, char* argv[])
{
	std::string filename = argv[1];
    std::cout << "Hello world" << std::endl;

    auto adj = Adjacency::create(filename);
    for (size_t i = 0 ; i < adj.count() ; ++i) {
        for (auto n : adj[i].to_vector()) {
            std::cout << n << ' ';
        }
        std::cout << "\n";
    }
}
