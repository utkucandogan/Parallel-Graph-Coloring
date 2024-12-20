#include "sequential.hpp"

void color_sequential(const Adjacency& adj, ColorArray& colors)
{
    ForbiddenColors forbiddenColors(colors.max_color());
    for (Adjacency::index_t vertex = 0 ; vertex < adj.count() ; ++vertex) {
        for (auto neighbor = adj[vertex] ; *neighbor != Adjacency::END ; ++neighbor) {
            forbiddenColors[colors[*neighbor]] = vertex;
        }
        for (ColorArray::color_t color = 1 ; color <= colors.max_color() ; ++color) {
            if (forbiddenColors[color] != vertex) {
                colors[vertex] = color;
                break;
            }
        }
    }
}