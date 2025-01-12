#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "Helper.hpp"
#include <iostream>

#define DEBUG 1
// Variable starting with p are process specific variables
int main(int argc, char* argv[])
{
    uint32_t vertex_count = 32;
    uint32_t max_degree = 4;
    bool use_broadcast = false;
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
    uint32_t p_vertex_count = vertex_count / size;
    uint32_t constant_vertex_count = vertex_count / size;
    // Used for MPI functions since the other one is updated throughout the code
    uint32_t p_vertex_count_init = vertex_count / size;
    // Start of the vertices according to each processes
    uint32_t p_vertex_start_index = rank * p_vertex_count;

    uint32_t* p_forbidden_colors = NULL;
    uint32_t* p_collision_array = NULL;
    uint32_t* p_adjacency_array = NULL;
    uint32_t* color_array = NULL;
    uint32_t* operating_processes_array = NULL;
    uint32_t* process_array = NULL; // hol which process needs which data

    if (!allocate_and_initialize(&p_forbidden_colors,
        &p_collision_array,
        &p_adjacency_array,
        &color_array,
        &process_array,
        &operating_processes_array,
        size,
        max_degree,
        p_vertex_count,
        vertex_count))
    {
        fprintf(stderr, "Failed to allocate resources.\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // Rank 0 allocates and initializes the vertex array
    uint32_t* adjacency_array = NULL;
    if (rank == 0) {
        adjacency_array = generate_adjency_array(max_degree, vertex_count);
        printf("size :%d  vertex_count:%d, max_degree:=%d pvertex_count:%d \n",size,vertex_count,max_degree,p_vertex_count);
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
        p_vertex_count_init * max_degree,       // how many elements to send to each rank
        MPI_UINT32_T,
        p_adjacency_array,   // receive buffer on each rank
        p_vertex_count_init * max_degree,
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
        //printf("Adjency array: \n");
        //print_adjency_array(adjacency_array, vertex_count, max_degree);
#endif // DEBUG
        delete [] adjacency_array;
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
    uint32_t global_collision_flag = 1;
    uint32_t itertion_count = 0;
    while (global_collision_flag) {
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgather(
            MPI_IN_PLACE, // "in-place" send buffer
            0,            // ignored
            MPI_DATATYPE_NULL, // ignored
            operating_processes_array, // receive buffer (must have N=local_vertex_count*size capacity)
            1,      // how many elements each rank contributes
            MPI_UINT32_T,
            MPI_COMM_WORLD
        );
        int active_process = operating_processes_array[rank] == 1 ? 1 :MPI_UNDEFINED;
        MPI_Comm active_com;
        MPI_Comm_split(MPI_COMM_WORLD, active_process, rank, &active_com);
        int active_rank=0;
        for(int i =0 ; i < size ; i ++){
            if(i == rank)
                break;
            if(operating_processes_array[i]==1)
                active_rank +=1;
        }
        
        //printf("entering process broadcast  rank :%d iteration :%d\n",rank,itertion_count);
        if(p_vertex_count){
            color_vertices_dyanmic(p_adjacency_array, color_array,process_array, p_forbidden_colors, p_collision_array, p_vertex_start_index,p_vertex_count, constant_vertex_count, max_degree);
            int rank_counter =0;
            for (int i = 0; i < size; i++) {
                
                if(operating_processes_array[i] == 0){
                    continue;
                }
                int membership = ((process_array[i]==1) || rank == i )? 1 : MPI_UNDEFINED;
                int master_rank = UINT32_MAX;
                int sub_rank=UINT32_MAX;
                MPI_Barrier(active_com);
                MPI_Comm sub_comm;
                //printf("reached the barrier  rank :%d iteration := %d membership :%d\n",rank,i,membership);
                MPI_Comm_split(active_com, membership, rank, &sub_comm);
                if(membership == 1)
                    MPI_Comm_rank(sub_comm, &sub_rank);
                if(i == rank)
                    master_rank = sub_rank;
                MPI_Bcast(&master_rank, 1, MPI_INT, rank_counter, active_com);
                rank_counter +=1;
                int data = 0;
                if(i == rank)
                    data=100/(i+1);
                if(membership==1){
                    MPI_Bcast(&color_array[i*constant_vertex_count], constant_vertex_count, MPI_INT, master_rank, sub_comm);
                    printf("itration :%d srank %d rank %d need data:= %d\n", i,sub_rank,rank ,color_array[i*constant_vertex_count]);
                    process_array[i]=0;
                }
                MPI_Barrier(active_com);
                //MPI_Comm_free(&sub_comm);
            }
        }
        MPI_Allgather(
            MPI_IN_PLACE, // "in-place" send buffer
            0,            // ignored
            MPI_DATATYPE_NULL, // ignored
            color_array, // receive buffer (must have N=local_vertex_count*size capacity)
            p_vertex_count_init,      // how many elements each rank contributes
            MPI_UINT32_T,
            MPI_COMM_WORLD
        );
#if DEBUG
        // --------------------------------------------
        // Print out the final color array on each rank
        // --------------------------------------------
        //printf("Rank %d color_array: ", rank);
        //for (size_t i = 0; i < vertex_count; i++) {
        //    printf("%u ", color_array[i]);
        //}
        //printf("\n");
        // --------------------------------------------
#endif // DEBUG
        if(p_vertex_count)
            check_collisions(p_adjacency_array, color_array, operating_processes_array, p_collision_array, rank,p_vertex_start_index, p_vertex_count, max_degree);

#if DEBUG
        // --------------------------------------------
        // Print out the final color array on each rank
        // --------------------------------------------
        printf("Rank %d collision count %d: iteration: %d ", rank, p_vertex_count,itertion_count);
        for (size_t i = 0; i < p_vertex_count; i++) {
            printf("%u ", p_collision_array[i] + p_vertex_start_index);
        }
        printf("\n");
        // --------------------------------------------
#endif // DEBUG
        
        // 5) Use an all-reduce to see if *any* rank has a collision
        MPI_Allreduce(
            &p_vertex_count,    // send buffer
            &global_collision_flag,   // receive buffer
            1,              // count
            MPI_UINT32_T,        // datatype
            MPI_MAX,        // operation (max will be 1 if any rank is 1)
            MPI_COMM_WORLD
        );
        itertion_count +=1;
    }
    //If global_collision_flag==0 => no collisions on any rank and loop is exited
    if (rank == 0) {
        printf("No collisions found; coloring is complete.\n");
    }
    // Free memory
    
    free(process_array);
    free(p_adjacency_array);
    free(color_array);
    free(p_forbidden_colors);
    MPI_Finalize();
    return 0;
}
