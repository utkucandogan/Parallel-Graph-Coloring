#include <iostream>

#include "adjacency.hpp"
#include "color.hpp"
#include "sequential.hpp"

int main(int argc, char* argv[])
{
    auto adj = Adjacency::create(argv[1]);
    adj.print();

    std::cout << "\n";

    ColorArray colors(adj.count(), adj.degree()+1);
    color_sequential(adj, colors);
    colors.print();

    if (check_colors(adj, colors)) {
        std::cout << "Coloring is done correctly.\n";
    } else {
        throw std::runtime_error("Coloring is done incorrectly!");
    }
}
