#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "adjacency.h"

// Shuffle a list with size n using Fisher–Yates method
void shuffle(uint32_t* array, size_t n)
{
    if (n <= 1) return;

    for (size_t i = 0 ; i < n - 1 ; ++i) {
        // Get a random index between i and n-1
        size_t j = i + rand() % (n - i);

        // Swap the elements at i and j
        uint32_t tmp = array[j];
        array[j] = array[i];
        array[i] = tmp;
    }
}

// Code to generate adjacency array
uint32_t* generate_adjacency_array(uint32_t max_degree, uint32_t vertex_count)
{
    // Initialize the adjacency matrix with zeros (no edges)
    uint32_t* matrix = malloc(max_degree * vertex_count * sizeof(uint32_t));

    // Initialize all entries to UINT32_MAX to indicate no connection
    for (uint32_t idx = 0 ; idx < max_degree * vertex_count ; ++idx) {
        matrix[idx] = UINT32_MAX;
    }

    // A list for generating random permutations
    uint32_t* randomVertices = malloc(vertex_count * sizeof(uint32_t));

    // Seed the random number generator
    srand(time(NULL));

    // Generate edges for each vertex
    for (uint32_t thisVertex = 0 ; thisVertex < vertex_count ; ++thisVertex) {
        // List the available vertices to connect to (excluding the current vertex and all the vertices before)
        for (size_t i = 0 ; i < vertex_count - thisVertex ; ++i) {
            randomVertices[i] = i + thisVertex + 1;
        }

        // Shuffle the available vertices to randomize the graph
        shuffle(randomVertices, vertex_count - thisVertex);

        // Randomly decide how many edges this vertex should have
        uint32_t degree = rand() % (max_degree + 1); // Random degree from 0 to max_degree

        // Find the first available point and decrease the degree along the way
        uint32_t* thisVertexNeighbors = &matrix[thisVertex * max_degree];
        uint32_t currentNeighbors = 0;
        while(*thisVertexNeighbors != UINT32_MAX) {
            ++thisVertexNeighbors;
            ++currentNeighbors;
        }

        // Skip adding new neighbors if already full
        if (currentNeighbors >= degree) continue;
        degree -= currentNeighbors;

        // Create edges for the current vertex and connect it the both ways
        for (uint32_t i = 0 ; i < degree ; ++i) {
            uint32_t neighborVertex = randomVertices[i];

            // Assign the neighbor and go to the next spot
            *(thisVertexNeighbors++) = neighborVertex;

            // Find the first available point for the neighbor
            uint32_t* neighborVertexNeighbors = &matrix[neighborVertex * max_degree];
            while(*neighborVertexNeighbors != UINT32_MAX) {
                ++neighborVertexNeighbors;
            }
            *neighborVertexNeighbors = thisVertex;
        }
    }

    // Free the shuffling array
    free(randomVertices);

    return matrix;
}


//Prints the adjacency array
void print_adjacency_array(uint32_t* matrix, uint32_t vertex_count, uint32_t max_degree)
{
    printf("=== ADJACENCY ARRAY ===\n");
    for (uint32_t i = 0 ; i < vertex_count ; ++i) {
        printf("\tVertex %u: ", i);
        for (uint32_t* neighbors = &matrix[i * max_degree] ; *neighbors != UINT32_MAX ; ++neighbors) {
            printf("%u ", *neighbors);
        }
        printf("\n");
    }
    printf("=======================\n");
}

// Read input file
// file is designed such that first read data is vertex count
// second is matrix depth, the rest are vertex_count*matrix_depth number of adjaency data
uint32_t* read_input(const char* filename, uint32_t* vertex_count, uint32_t* max_degree)
{
    // Open the file
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    // Read the number of vertices and max array size
    fread(vertex_count, sizeof(uint32_t), 1, file);
    fread(max_degree, sizeof(uint32_t), 1, file);

    uint32_t total_size = (*vertex_count) * (*max_degree);

    uint32_t* matrix = (uint32_t*) malloc(total_size * sizeof(uint32_t));

    printf("Matrix size: %d Max Degree: %d\n", *vertex_count, *max_degree);

    uint32_t data;
    uint32_t location_counter = 0;
    while (fread(&data, sizeof(uint32_t), 1, file)) {
        matrix[location_counter] = data;
        if(location_counter == (total_size-1))
            break;
        location_counter += 1;
    }

    fclose(file);
    return matrix;
}
