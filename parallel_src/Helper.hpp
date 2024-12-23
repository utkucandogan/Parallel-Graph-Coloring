#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>

using namespace std;

//Colors the specified number of vertices using a dynamic array
void color_vertices_dyanmic(uint32_t* p_adjacency_array, uint32_t* color_array, uint32_t* forbidden_colors, uint32_t* p_collision_array,
    uint32_t p_vertex_start_index, uint32_t vertex_count, uint32_t max_degree) {

    uint32_t neighbour;
    uint32_t neighbour_offset = 0;
    //For each vertex look at all the neighbours, forbid their colors select min unforbid color
    for (size_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index) {
        uint32_t vertex = p_collision_array[vertex_index];
        if (vertex == UINT32_MAX)
            break;
        for (size_t neighbour_index = 0; neighbour_index < max_degree; ++neighbour_index) {
            neighbour = p_adjacency_array[neighbour_offset + neighbour_index];
            if (neighbour == UINT32_MAX)
                break;
            else
                //Color_array is zero-indexed, forbidden colors are one-indexed
                forbidden_colors[color_array[neighbour]] = vertex;
        }
        //Select min unforbid color for the current vertex
        for (uint32_t color = 1; color <= max_degree + 1; ++color) {
            if (forbidden_colors[color] != vertex) {
                color_array[p_vertex_start_index + vertex] = color;
                break;
            }
        }
        //Update offset to the next neighbour
        neighbour_offset += max_degree;
    }
}

uint32_t* generate_adjency_array(uint32_t max_degree, uint32_t vertex_count) {
    // Initialize the adjacency matrix with zeros (no edges)
    uint32_t* matrix = new uint32_t[max_degree * vertex_count]();

    // Initialize all entries to UINT32_MAX
    for (uint32_t idx = 0; idx < max_degree * vertex_count; ++idx) {
        matrix[idx] = UINT32_MAX;
    }

    // Seed the random number generator
    unsigned seed = static_cast<unsigned>(time(0));
    default_random_engine engine(seed);

    // Generate edges for each vertex
    for (uint32_t i = 0; i < vertex_count; ++i) {
        // List of available vertices to connect to (excluding the current vertex)
        vector<int> available_vertices;
        for (uint32_t j = 0; j < vertex_count; ++j) {
            if (j != i) {
                available_vertices.push_back(j);
            }
        }

        // Shuffle the available vertices to randomize the graph
        shuffle(available_vertices.begin(), available_vertices.end(), engine);

        // Randomly decide how many edges this vertex should have
        uint32_t degree = rand() % (max_degree + 1); // Random degree from 0 to max_degree

        // Create edges for the current vertex
        for (uint32_t k = 0; k < degree; ++k) {
            if (!available_vertices.empty()) {
                // Pick a vertex from the shuffled list of available vertices
                int connected_vertex = available_vertices.back();
                available_vertices.pop_back();

                // Add the connected vertex to the matrix (use 1-based indexing)
                matrix[i * max_degree + k] = connected_vertex + 1;
            }
        }
    }

    return matrix;
}
void print_adjency_array(uint32_t* matrix, uint32_t vertex_count, uint32_t max_degree) {
    for (uint32_t i = 0; i < vertex_count; ++i) {
        for (uint32_t k = 0; k < max_degree; ++k) {
            std::cout << matrix[i * max_degree + k] << " ";
        }
        std::cout << std::endl;
    }
}