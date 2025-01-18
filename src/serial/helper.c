#include <stdio.h>
#include <stdlib.h>
#include "helper.h"

//Colors the specified number of vertices using a dynamic array
void color_vertices_dynamic(
    uint32_t* p_adjacency_array,
    uint32_t* color_array,
    uint32_t* process_array,
    uint32_t* forbidden_colors,
    uint32_t* p_collision_array,
    uint32_t p_vertex_start_index,
    uint32_t vertex_count,
    uint32_t constant_vertex_count,
    uint32_t max_degree
)
{
    uint32_t neighbour;
    uint32_t neighbour_offset = 0;
    //For each vertex look at all the neighbours, forbid their colors select min unforbid color
    for (size_t vertex_index = 0 ; vertex_index < vertex_count ; ++vertex_index) {
        // Read the local vertex index from the set of vertices with collisions
        uint32_t vertex = p_collision_array[vertex_index];
        //Update offset to the next neighbour, not optimized can definitely be a better solution
        neighbour_offset = vertex * max_degree;

        if (vertex == UINT32_MAX) break;

        for (size_t neighbour_index = 0 ; neighbour_index < max_degree ; ++neighbour_index) {
            neighbour = p_adjacency_array[neighbour_offset + neighbour_index];
            if (neighbour == UINT32_MAX)
                break;
            else{
                uint32_t write_index = neighbour/(constant_vertex_count);
                //printf("neighbor := %d write_index %d\n",neighbour,write_index);
                process_array[write_index] = 1;
                //Color_array is zero-indexed, forbidden colors are one-indexed
                // Use vertex + 1 for forbidding since 0 is used for uncolored at the beginining
                forbidden_colors[color_array[neighbour]] = vertex + 1;
            }
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

// Colors the specified number of vertices using a dynamic array
bool check_collisions(
    ref_type(uint32_t*) p_collision_array,
    ref_type(uint32_t) p_vertex_count,
    uint32_t* p_adjacency_array,
    uint32_t* color_array,
    uint32_t* operating_processes_array,
    uint32_t rank,
    uint32_t p_vertex_start_index,
    uint32_t max_degree
)
{
    uint32_t* p_new_collision_array = (uint32_t*) malloc(ref(p_vertex_count) * sizeof(uint32_t));
    if (!p_new_collision_array) {
        perror("Allocation Error for p_new_collision_array");
        return false;
    }

    // Reset the collision count
    uint32_t p_new_vertex_count = 0;

    // For each vertex look at all the neighbours, forbid their colors select min unforbid color
    for (size_t vertex_index = 0 ; vertex_index < ref(p_vertex_count) ; ++vertex_index) {
        uint32_t vertex = ref(p_collision_array)[vertex_index];

        if (vertex == UINT32_MAX) break;

        // Loop neighbors until UINT32_MAX reached
        for (uint32_t* neighbor = &p_adjacency_array[vertex * max_degree] ; *neighbor != UINT32_MAX ; ++neighbor) {
            // A collision has been detected
            if (color_array[*neighbor] == color_array[p_vertex_start_index + vertex]) {
                // If the vertex is bigger add, o.w. do not add but stop all the time
                if ((p_vertex_start_index + vertex) > *neighbor) {
                    p_new_collision_array[p_new_vertex_count++] = vertex;
                }
                // Avoids duplicate addition of the same vertex if it has multiple collisions
                break;
            }
        }
    }

    //Update the number of vertices to be handled and their indexes
    if (p_new_vertex_count < ref(p_vertex_count)) {
        p_new_collision_array[p_new_vertex_count] = UINT32_MAX;
    }

    ref(p_vertex_count) = p_new_vertex_count;

    if(p_new_vertex_count == 0) {
        operating_processes_array[rank] = 0;
    }

    free(ref(p_collision_array));
    ref(p_collision_array) = p_new_collision_array;

    // Success
    return true;
}

//Allocates and initalizes all the necessary variables
bool allocate_and_initialize(
    ref_type(uint32_t*) p_forbidden_colors,
    ref_type(uint32_t*) p_collision_array,
    ref_type(uint32_t*) p_adjacency_array,
    ref_type(uint32_t*) color_array,
    size_t size,
    size_t max_degree,
    size_t p_vertex_count,
    size_t vertex_count
)
{
    // 0) Check if inputs are null
    if (!p_forbidden_colors || !p_collision_array || !p_adjacency_array || !color_array) {
        perror("All inputs must be non-NULL");
        return false;  // Nothing else to free yet
    }

    // 1) Allocate forbidden colors
    ref(p_forbidden_colors) = (uint32_t*) malloc((max_degree + 2) * sizeof(uint32_t));
    if (!ref(p_forbidden_colors)) {
        perror("Could not allocate p_forbidden_colors");
        return false;  // Nothing else to free yet
    }
    // Initialize forbidden colors to 0
    for (size_t i = 0 ; i < max_degree + 2 ; ++i) {
        ref(p_forbidden_colors)[i] = 0;
    }

    // 2) Allocate collision array
    ref(p_collision_array) = (uint32_t*) malloc(p_vertex_count * sizeof(uint32_t));
    if (!ref(p_collision_array)) {
        perror("Could not allocate p_collision_array");
        free(ref(p_forbidden_colors));
        ref(p_forbidden_colors) = NULL;
        return false;
    }
    // Initialize collision array
    for (size_t i = 0; i < p_vertex_count; ++i) {
        ref(p_collision_array)[i] = (uint32_t) i;
    }

    // 3) Allocate adjacency array
    ref(p_adjacency_array) = (uint32_t*) malloc(p_vertex_count * max_degree * sizeof(uint32_t));
    if (!*p_adjacency_array) {
        perror("Could not allocate p_adjacency_array");
        // Free already allocated arrays
        free(ref(p_forbidden_colors));
        ref(p_forbidden_colors) = NULL;
        free(ref(p_collision_array));
        ref(p_collision_array) = NULL;
        return false;
    }
    // No specific initialization given for adjacency array,
    // but you can initialize here if needed.

    // 4) Allocate color array
    ref(color_array) = (uint32_t*) malloc(vertex_count * sizeof(uint32_t));
    if (!ref(color_array)) {
        fprintf(stderr, "Error: Could not allocate color_array.\n");
        // Free already allocated arrays
        free(ref(p_forbidden_colors));
        ref(p_forbidden_colors) = NULL;
        free(ref(p_collision_array));
        ref(p_collision_array) = NULL;
        free(ref(p_adjacency_array));
        ref(p_adjacency_array) = NULL;
        return false;
    }
    // Initialize color array to 0
    for (size_t i = 0; i < vertex_count; i++) {
        ref(color_array)[i] = 0;
    }

    return true;  // Allocation and initialization succeeded
}
