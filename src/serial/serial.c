#include <stdio.h>
#include <stdlib.h>

#include "serial.h"


//Colors the specified number of vertices using a dynamic array
void color_vertices_static(uint32_t* adjacency_array, uint32_t* color_array, uint32_t* forbidden_colors, uint32_t vertex_count, uint32_t max_degree) {

    uint32_t neighbour;
    uint32_t neighbour_offset = 0;
    //For each vertex look at all the neighbours, forbid their colors select min unforbid color
    for (size_t vertex = 0; vertex < vertex_count; ++vertex) {
        for (size_t neighbour_index = 0; neighbour_index < max_degree; ++neighbour_index) {
            neighbour = adjacency_array[neighbour_offset + neighbour_index];
            if (neighbour == UINT32_MAX)
                break;
            else
                //Color_array is zero-indexed, forbidden colors are one-indexed
                // Use vertex + 1 for forbidding since 0 is used for uncolored at the beginining
                forbidden_colors[color_array[neighbour]] = vertex + 1;
        }
        //Select min unforbid color for the current vertex
        for (uint32_t color = 1; color <= max_degree + 1; ++color) {
            if (forbidden_colors[color] != vertex + 1) {
                color_array[vertex] = color;
                break;
            }
        }
        //Update offset to the next neighbour
        neighbour_offset += max_degree;
    }
}

bool serial_color(uint32_t vertex_count, uint32_t max_degree, uint32_t* adjacency_array)
{

    uint32_t* forbidden_colors = NULL;
    uint32_t* color_array = NULL;
    forbidden_colors = (uint32_t*)malloc((max_degree + 1) * sizeof(uint32_t));
    if (!forbidden_colors) {
        fprintf(stderr, "Error: Could not allocate p_forbidden_colors.\n");
        return false;  // Nothing else to free yet
    }
    color_array = (uint32_t*)malloc(vertex_count * sizeof(uint32_t));
    if (!color_array) {
        fprintf(stderr, "Error: Could not allocate color_array.\n");
        // Free already allocated arrays
        free(forbidden_colors);   forbidden_colors = NULL;
        return false;
    }
    color_vertices_static(
        adjacency_array,
        color_array,
        forbidden_colors,
        vertex_count,
        max_degree
    );

    // Free memory
    free(forbidden_colors);
    free(color_array);
    return true;
}
