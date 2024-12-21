#include "sequential.hpp"

void color_sequential(const Adjacency& adj, ColorArray& colors)
{
    ForbiddenColors forbiddenColors(colors.max_color());
    // Iterate over each vertex indices
    for (Adjacency::index_t vertex = 0 ; vertex < adj.count() ; ++vertex) {
        // Iterate over each vertices neighbor array using pointer arithmetic,
        // Each pointer points to a vertex index
        for (auto neighbor = adj[vertex] ; *neighbor != Adjacency::END ; ++neighbor) {
            // Forbid the color of the neighbor vertices
            forbiddenColors[colors[*neighbor]] = vertex;
        }
        // Assign the minimum numbered color that no neighbor has
        for (ColorArray::color_t color = 1 ; color <= colors.max_color() ; ++color) {
            if (forbiddenColors[color] != vertex) {
                colors[vertex] = color;
                break;
            }
        }
    }
}
