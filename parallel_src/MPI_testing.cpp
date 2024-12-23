#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "helper.hpp"

#define DEBUG 0
// Variable starting with p are process specific variables
int main(int argc, char* argv[])
{
    uint32_t vertex_count = 16;
    uint32_t max_degree = 4;
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if ((size%2)== 1) {
        if (rank == 0) {
            printf("Please run this with power of 2 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // p_vertex_count indicates how many vertices each process will handle
    uint32_t p_vertex_count = vertex_count / size;
    // Start of the vertices according to each processes
    uint32_t p_vertex_start_index = rank * p_vertex_count;

    //Initialize the forbidden colors to all false
    // Max_degree + 1 colors for the worst case and 0 means uncolored
    uint32_t* p_forbidden_colors = new uint32_t[max_degree + 2];
    for (size_t i = 0; i < max_degree + 2; i++) {
        p_forbidden_colors[i] = 0;
    }

    //Initialize collisions to all
    // Max_degree + 1 colors for the worst case and 0 means uncolored
    uint32_t* p_collision_array = new uint32_t[p_vertex_count];
    uint32_t p_collision_count = p_vertex_count;
    for (size_t i = 0; i < p_vertex_count; ++i) {
        p_collision_array[i] = i;
    }

    // Allocate buffer for each rank to store its local vertices
    uint32_t* p_adjacency_array = new uint32_t[p_vertex_count*max_degree];

    // Allocate buffer for each rank to store all the colors
    uint32_t* color_array = new uint32_t[vertex_count];
    for (size_t i = 0; i < vertex_count; i++) {
        color_array[i] = 0;
    }

    // Rank 0 allocates and initializes the vertex array
    uint32_t* adjacency_array = NULL;
    if (rank == 0) {
        adjacency_array = generate_adjency_array(max_degree, vertex_count);
    }

    // Scatter the big array to all processes
    //  - On rank 0, MPI_Scatter will send the appropriate chunk to each rank
    //  - On other ranks, MPI_Scatter will receive that chunk
    // The signature:
    //   MPI_Scatter(sendbuf, sendcount, sendtype,
    //               recvbuf, recvcount, recvtype,
    //               root, comm)
    MPI_Scatter(
        adjacency_array,     // send buffer (only significant at root=0)
        p_vertex_count * max_degree,       // how many elements to send to each rank
        MPI_UINT32_T,
        p_adjacency_array,   // receive buffer on each rank
        p_vertex_count * max_degree,
        MPI_UINT32_T,
        0,             // root (which holds the big array)
        MPI_COMM_WORLD
    );

    // After scatter, each rank has its own slice in p_adjacency_array
    // For example, rank 0 has elements [0,1,2,3],
    // rank 1 has [4,5,6,7], rank 2 has [8,9,10,11], and rank 3 has [12,13,14,15].

    // Now it's safe to free adjacency_array on rank 0
    if (rank == 0) {
#if DEBUG
        print_adjency_array(adjacency_array, vertex_count, max_degree);
#endif // DEBUG
        free(adjacency_array);
        adjacency_array = NULL;
    }
#if DEBUG
    // Print out what each rank received
    printf("Rank %d received: ", rank);
    for (int i = 0; i < p_vertex_count * max_degree; i++) {
        printf("%d ", p_adjacency_array[i]);
    }
    printf("\n");
#endif // DEBUG,
    int global_flag = 1;
    int local_flag = 1;
    while(global_flag){
    color_vertices_dyanmic(p_adjacency_array, color_array, p_forbidden_colors, p_collision_array, p_vertex_start_index, p_collision_count, max_degree);

        MPI_Allgather(
            MPI_IN_PLACE, // "in-place" send buffer
            0,            // ignored
            MPI_DATATYPE_NULL, // ignored
            color_array, // receive buffer (must have N=local_vertex_count*size capacity)
            p_vertex_count,      // how many elements each rank contributes
            MPI_UINT32_T,
            MPI_COMM_WORLD
        );
    #if DEBUG
        // --------------------------------------------
        // Print out the final color array on each rank
        // --------------------------------------------
        printf("Rank %d color_array: ", rank);
        for (size_t i = 0; i < vertex_count; i++) {
            printf("%u ", color_array[i]);
        }
        printf("\n");
        // --------------------------------------------
    #endif // DEBUG

        // 3) Check for local collisions
            //    This depends on how you define a "collision."
            //    For instance, if any pair of adjacent vertices share the same color
            //    in color_array, that's a collision. 
            //    We'll do a pseudo-check:
        //Reset the collision array
        p_collision_count = 0;
        for (size_t i = 0; i < p_vertex_count; ++i) {
            p_collision_array[i] = i;
        }
        local_flag = 0;
        uint32_t neighbour;
        uint32_t neighbour_offset = 0;
        //For each vertex look at all the neighbours, forbid their colors select min unforbid color
        for (size_t vertex_index = 0; vertex_index < p_vertex_count; ++vertex_index) {
            uint32_t global_vertex_id = (rank * p_vertex_count) + vertex_index;
            uint32_t vertex = p_collision_array[vertex_index];
            if (vertex == UINT32_MAX)
                break;
            for (size_t neighbour_index = 0; neighbour_index < max_degree; ++neighbour_index) {
                neighbour = p_adjacency_array[neighbour_offset + neighbour_index];
                if (neighbour == UINT32_MAX)
                    break;
                else if(color_array[neighbour] == color_array[global_vertex_id])
                    //Color_array is zero-indexed, forbidden colors are one-indexed
                    
            }
            //Select min unforbid color for the current vertex
            for (uint32_t color = 1; color <= max_degree + 1; ++color) {
                if (p_forbidden_colors[color] != vertex) {
                    color_array[p_vertex_start_index + vertex] = color;
                    break;
                }
            }
            //Update offset to the next neighbour
            neighbour_offset += max_degree;
        }

        // 5) Use an all-reduce to see if *any* rank has a collision
        int global_flag = 0;
        MPI_Allreduce(
            &local_flag,    // send buffer
            &global_flag,   // receive buffer
            1,              // count
            MPI_INT,        // datatype
            MPI_MAX,        // operation (max will be 1 if any rank is 1)
            MPI_COMM_WORLD
        );

        // 6) If global_flag==0 => no collisions on any rank, so we can stop
        if (global_flag == 0) {
            if (rank == 0) {
                printf("No collisions found; coloring is complete.\n");
            }
        }
        else {
            // collisions exist (global_flag==1)
            if (rank == 0) {
                printf("Collision detected; re-running the coloring.\n");
            }
            // Optionally reinitialize color_array or partial data here if needed
            // Then loop again to do another iteration
        }
    }
    // Free memory
    free(p_adjacency_array);
    free(color_array);
    free(p_forbidden_colors);
    MPI_Finalize();
    return 0;
}
