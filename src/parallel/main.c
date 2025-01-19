#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "adjacency.h"
#include "color.h"
#include "scheduler.h"

// Variable starting with p are process specific variables
int main(int argc, char* argv[])
{
    uint32_t vertex_count;
    uint32_t max_degree;
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /*if ((size % 2) == 1) {
        if (rank == 0) {
            printf("Please run this with power of 2 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }*/

    // p_vertex_count indicates how many vertices each process will handle
    uint32_t p_vertex_count;
    // Used for MPI functions since the other one is updated throughout the code
    uint32_t p_vertex_count_init;
    // Start of the vertices according to each processes
    uint32_t p_vertex_start_index;

    uint32_t* p_forbidden_colors = NULL;
    uint32_t* p_collision_array = NULL;
    uint32_t* p_adjacency_array = NULL;
    uint32_t* color_array = NULL;
    uint32_t* adjacency_array = NULL;

    // Method specific arrays
    uint32_t* operating_processes_array = NULL;
    uint32_t* process_array = NULL; // holds which process needs which data

    bool use_scheduler = false;
    // Select method
    if (strcmp(argv[1], "broadcaster") == 0) {
        use_scheduler = false;
    } else if (strcmp(argv[1], "scheduler") == 0) {
        use_scheduler = true;
    } else {
        fprintf(stderr, "Selected method is not valid!\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // read input
    if (rank == 0) {
        adjacency_array = read_input(argv[2], &vertex_count, &max_degree);
        // adjacency_array = generate_adjency_array(max_degree, vertex_count);
        printf("Size: %d, Vertex Count: %d, Max Degree: %d\n", size, vertex_count, max_degree);
    }
    // broacast input matrix parameters
    MPI_Bcast(&max_degree, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // printf("bcast done rank:%d max_degree:%d\n",rank,max_degree);
    MPI_Bcast(&vertex_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // printf("bcast done rank:%d vertex_count:%d\n",rank,vertex_count);

    // initialize variables according to inputs
    p_vertex_count = vertex_count / size;
    p_vertex_count_init = vertex_count / size;
    p_vertex_start_index = rank * p_vertex_count;

    if (!allocate_and_initialize(
            &p_forbidden_colors,
            &p_collision_array,
            &p_adjacency_array,
            &color_array,
            &process_array,
            &operating_processes_array,
            size,
            max_degree,
            p_vertex_count,
            vertex_count
    )) {
        fprintf(stderr, "Failed to allocate resources.\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // Rank 0 allocates and initializes the vertex array

    MPI_Scatter(
        adjacency_array,                  // send buffer (only significant at root=0)
        p_vertex_count_init * max_degree, // how many elements to send to each rank
        MPI_UINT32_T,
        p_adjacency_array,                // receive buffer on each rank
        p_vertex_count_init * max_degree,
        MPI_UINT32_T,
        0,                                // root (which holds the big array)
        MPI_COMM_WORLD
    );

    // Now it's safe to free adjacency_array on rank 0
    if (rank == 0) {

#ifdef DEBUG
        printf("Adjacency array: \n");
        print_adjacency_array(adjacency_array, vertex_count, max_degree);
#endif // DEBUG

        free(adjacency_array);
        adjacency_array = NULL;
    }

    // measure start time
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

#ifdef DEBUG
    // Print out what each rank received
    printf("Rank %d received: ", rank);
    for (int i = 0; i < p_vertex_count * max_degree; i++) {
        printf("%d ", p_adjacency_array[i]);
    }
    printf("\n");
#endif // DEBUG,

    uint32_t global_collision_flag = 1;
    uint32_t itertion_count = 0;
    while (global_collision_flag) {
        MPI_Comm active_comm = use_scheduler ? scheduler_get_active_comm(rank, size, operating_processes_array) : 0;

        if(p_vertex_count) {
            color_vertices_dynamic(
                p_adjacency_array,
                color_array,
                p_forbidden_colors,
                p_collision_array,
                process_array,
                p_vertex_start_index,
                p_vertex_count,
                p_vertex_count_init,
                max_degree
            );
            if (use_scheduler) scheduler_broadcast(rank, size, process_array, operating_processes_array, active_comm, p_vertex_count_init, color_array);
        }

#ifdef DEBUG
        // Print out the final color array on each rank
        printf("Rank %d, color_array: ", rank);
        for (size_t i = 0; i < vertex_count; i++) {
            printf("%u ", color_array[i]);
        }
        printf("\n");
#endif // DEBUG

        if (!use_scheduler) {
            MPI_Allgather(
                MPI_IN_PLACE,        // "in-place" send buffer
                0,                   // ignored
                MPI_DATATYPE_NULL,   // ignored
                color_array,         // receive buffer (must have N=local_vertex_count*size capacity)
                p_vertex_count_init, // how many elements each rank contributes
                MPI_UINT32_T,
                MPI_COMM_WORLD
            );
        }

        if (p_vertex_count && !check_collisions(
            &p_collision_array,
            &p_vertex_count,
            p_adjacency_array,
            color_array,
            operating_processes_array,
            rank,
            p_vertex_start_index,
            max_degree
        )) {
            fprintf(stderr, "Failed to allocate resources during collision check.\n");
            goto EXIT;
        }

#ifdef DEBUG
        // Print out the final color array on each rank
        printf("Rank %d, collision count: %d, iteration: %d ", rank, p_vertex_count,itertion_count);
        for (size_t i = 0; i < p_vertex_count; i++) {
            printf("%u ", p_collision_array[i] + p_vertex_start_index);
        }
        printf("\n");
#endif // DEBUG

        // 5) Use an all-reduce to see if any rank has a collision
        MPI_Allreduce(
            &p_vertex_count,        // send buffer
            &global_collision_flag, // receive buffer
            1,                      // count
            MPI_UINT32_T,           // datatype
            MPI_MAX,                // operation (max will be 1 if any rank is 1)
            MPI_COMM_WORLD
        );
        itertion_count +=1;
    }
    //If global_collision_flag==0 => no collisions on any rank and loop is exited
    if (rank == 0) {
        printf("No collisions found; coloring is complete.\n");
    }

EXIT:
    // Free memory
    free(p_forbidden_colors);
    free(p_collision_array);
    free(p_adjacency_array);
    free(color_array);
    free(process_array);
    free(operating_processes_array);

    double end_time = MPI_Wtime();
    double max_time;
    double elapsed_time = end_time - start_time;
    MPI_Reduce(&elapsed_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Maximum elapsed time across all processes: %f seconds\n", max_time);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
