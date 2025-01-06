#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
//mpiexec -n 4 MPI_testing.exe

using namespace std;

//Colors the specified number of vertices using a dynamic array
void color_vertices_dyanmic(uint32_t* p_adjacency_array, uint32_t* color_array, uint32_t* forbidden_colors, uint32_t* p_collision_array,
    uint32_t p_vertex_start_index, uint32_t vertex_count, uint32_t max_degree) {

    uint32_t neighbour;
    uint32_t neighbour_offset = 0;
    //For each vertex look at all the neighbours, forbid their colors select min unforbid color
    for (size_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index) {
        // Read the local vertex index from the set of vertices with collisions
        uint32_t vertex = p_collision_array[vertex_index];
        //Update offset to the next neighbour, not optimized can definitely be a better solution
        neighbour_offset = vertex * max_degree;
        if (vertex == UINT32_MAX)
            break;
        for (size_t neighbour_index = 0; neighbour_index < max_degree; ++neighbour_index) {
            neighbour = p_adjacency_array[neighbour_offset + neighbour_index];
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
                color_array[p_vertex_start_index + vertex] = color;
                break;
            }
        }
    }
}


//Colors the specified number of vertices using a dynamic array
bool check_collisions(uint32_t* p_adjacency_array, uint32_t* color_array, uint32_t*& p_collision_array,
    uint32_t p_vertex_start_index, uint32_t &p_vertex_count, uint32_t max_degree) {
    uint32_t neighbour;
    uint32_t* p_new_collision_array = (uint32_t*)malloc(p_vertex_count * sizeof(uint32_t));
    if (!p_new_collision_array) {
        fprintf(stderr, "Error: Could not allocate p_new_collision_array.\n");
        return false;
    }
    uint32_t neighbour_offset = 0;
    uint32_t id_sum = 0;
    //Reset the collision count
    uint32_t p_new_vertex_count = 0;
    //For each vertex look at all the neighbours, forbid their colors select min unforbid color
    for (size_t vertex_index = 0; vertex_index < p_vertex_count; ++vertex_index) {
        uint32_t vertex = p_collision_array[vertex_index];
        //Update offset to the next neighbour, not optimized can definitely be a better solution
        neighbour_offset = vertex * max_degree;
        if (vertex == UINT32_MAX)
            break;
        for (size_t neighbour_index = 0; neighbour_index < max_degree; ++neighbour_index) {
            neighbour = p_adjacency_array[neighbour_offset + neighbour_index];
            if (neighbour == UINT32_MAX)
                break;
            // a collision has been detected
            else if (color_array[neighbour] == color_array[p_vertex_start_index + vertex]){
                // If the vertex is bigger add, o.w. do not add but stop all the time
                if ((p_vertex_start_index + vertex) > neighbour) {
                    p_new_collision_array[p_new_vertex_count++] = vertex;
                }
                // Avoids duplicate addition of the same vertex if it has multiple collisions
                break;
            }
        }
    }
    //Update the number of vertices to be handled and their indexes
    if(p_new_vertex_count<p_vertex_count)
        p_new_collision_array[p_new_vertex_count] = UINT32_MAX;
    p_vertex_count = p_new_vertex_count;
    free(p_collision_array);
    p_collision_array = p_new_collision_array;
    return true;
}

//Allocates and initalizes all the necessary variables
bool allocate_and_initialize(
    uint32_t** p_forbidden_colors,
    uint32_t** p_collision_array,
    uint32_t** p_adjacency_array,
    uint32_t** color_array,
    size_t     max_degree,
    size_t     p_vertex_count,
    size_t     vertex_count
) {
    // 1) Allocate forbidden colors
    *p_forbidden_colors = (uint32_t*)malloc((max_degree + 2) * sizeof(uint32_t));
    if (!*p_forbidden_colors) {
        fprintf(stderr, "Error: Could not allocate p_forbidden_colors.\n");
        return false;  // Nothing else to free yet
    }
    // Initialize forbidden colors to 0
    for (size_t i = 0; i < max_degree + 2; i++) {
        (*p_forbidden_colors)[i] = 0;
    }

    // 2) Allocate collision array
    *p_collision_array = (uint32_t*)malloc(p_vertex_count * sizeof(uint32_t));
    if (!*p_collision_array) {
        fprintf(stderr, "Error: Could not allocate p_collision_array.\n");
        free(*p_forbidden_colors);
        *p_forbidden_colors = NULL;
        return false;
    }
    // Initialize collision array
    for (size_t i = 0; i < p_vertex_count; ++i) {
        (*p_collision_array)[i] = (uint32_t)i;
    }

    // 3) Allocate adjacency array
    *p_adjacency_array = (uint32_t*)malloc(p_vertex_count * max_degree * sizeof(uint32_t));
    if (!*p_adjacency_array) {
        fprintf(stderr, "Error: Could not allocate p_adjacency_array.\n");
        // Free already allocated arrays
        free(*p_forbidden_colors);   *p_forbidden_colors = NULL;
        free(*p_collision_array);    *p_collision_array = NULL;
        return false;
    }
    // No specific initialization given for adjacency array, 
    // but you can initialize here if needed.

    // 4) Allocate color array
    *color_array = (uint32_t*)malloc(vertex_count * sizeof(uint32_t));
    if (!*color_array) {
        fprintf(stderr, "Error: Could not allocate color_array.\n");
        // Free already allocated arrays
        free(*p_forbidden_colors);   *p_forbidden_colors = NULL;
        free(*p_collision_array);    *p_collision_array = NULL;
        free(*p_adjacency_array);    *p_adjacency_array = NULL;
        return false;
    }
    // Initialize color array to 0
    for (size_t i = 0; i < vertex_count; i++) {
        (*color_array)[i] = 0;
    }

    return true;  // Allocation and initialization succeeded
}


// Code to generate adjacency array might not be 100% correct
uint32_t* generate_adjency_array(uint32_t max_degree, uint32_t vertex_count) {
    // Initialize the adjacency matrix with zeros (no edges)
    uint32_t* matrix = new uint32_t[max_degree * vertex_count];

    // Initialize all entries to UINT32_MAX to indicate no connection
    for (uint32_t idx = 0; idx < max_degree * vertex_count; ++idx) {
        matrix[idx] = UINT32_MAX;
    }

    // Create a set to track edges and avoid duplicates
    vector<vector<bool>> edge_exists(vertex_count, vector<bool>(vertex_count, false));

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

                // Check if the edge already exists in the reverse direction
                if (!edge_exists[i][connected_vertex]) {
                    // Add the edge from i to connected_vertex
                    for (uint32_t m = 0; m < max_degree; ++m) {
                        if (matrix[i * max_degree + m] == UINT32_MAX) {
                            matrix[i * max_degree + m] = connected_vertex;
                            break;
                        }
                    }

                    // Add the reverse edge from connected_vertex to i
                    for (uint32_t m = 0; m < max_degree; ++m) {
                        if (matrix[connected_vertex * max_degree + m] == UINT32_MAX) {
                            matrix[connected_vertex * max_degree + m] = i;
                            break;
                        }
                    }

                    // Mark both directions as having edges
                    edge_exists[i][connected_vertex] = true;
                    edge_exists[connected_vertex][i] = true;
                }
            }
        }
    }

    return matrix;
}


//Prints the adjacency array
void print_adjency_array(uint32_t* matrix, uint32_t vertex_count, uint32_t max_degree) {
    for (uint32_t i = 0; i < vertex_count; ++i) {
        printf("vertex %d: ", i);
        for (uint32_t k = 0; k < max_degree; ++k) {
            printf("%d ", matrix[i * max_degree + k]);
        }
        printf("\n");
    }
}