#include <stdio.h>
#include <stdlib.h>
#include "methods.h"

METHOD_INITIALIZE(scheduler)
{
    if (!process_array || !operating_processes_array) {
        perror("All inputs must be non-NULL");
        return false;  // Nothing else to free yet
    }
    ref(process_array) = (uint32_t*) malloc(size * sizeof(uint32_t));
    if (!ref(process_array)) {
        fprintf(stderr, "Error: Could not allocate process_array.\n");
        return false;
    }

    ref(operating_processes_array) = (uint32_t*) malloc(size * sizeof(uint32_t));
    if (!ref(operating_processes_array)) {
        fprintf(stderr, "Error: Could not allocate process_array.\n");
        // Free already allocated arrays
        free(ref(process_array));
        ref(process_array) = NULL;
        return false;
    }

    for (size_t i = 0 ; i < size ; i++) {
        ref(process_array)[i] = 0;
        ref(operating_processes_array)[i] = 1;
    }

    return true;
}

METHOD_FINALIZE(scheduler)
{
    free(ref(process_array));
    ref(process_array) = NULL;
    free(ref(operating_processes_array));
    ref(operating_processes_array) = NULL;
}

METHOD_PRECOLOR(scheduler)
{
    // MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allgather(
        MPI_IN_PLACE,              // "in-place" send buffer
        0,                         // ignored
        MPI_DATATYPE_NULL,         // ignored
        operating_processes_array, // receive buffer (must have N=local_vertex_count*size capacity)
        1,                         // how many elements each rank contributes
        MPI_UINT32_T,
        MPI_COMM_WORLD
    );


    int active_process = operating_processes_array[rank] == 1 ? 1 :MPI_UNDEFINED;
    MPI_Comm active_com;
    MPI_Comm_split(MPI_COMM_WORLD, active_process, rank, &active_com);


    int active_rank = 0;
    for(int i = 0 ; i < size ; ++i){
        if(i == rank) break;
        if(operating_processes_array[i] == 1) {
            active_rank += 1;
        }
    }

    return active_com;
}

METHOD_POSTCOLOR(scheduler)
{
    int rank_counter = 0;
    // Every active process will send its part of the color array to the needing processes
    for (int i = 0; i < size; i++) {
        if(operating_processes_array[i] == 0){
            continue;
        }
        int membership = ((process_array[i]==1) || rank == i )? 1 : MPI_UNDEFINED;
        int master_rank = UINT32_MAX;
        int sub_rank = UINT32_MAX;
        // MPI_Barrier(active_com);
        // create a communication group
        MPI_Comm sub_comm;
        MPI_Comm_split(active_com, membership, rank, &sub_comm);
        if(membership == 1)
            MPI_Comm_rank(sub_comm, &sub_rank);
        if(i == rank)
            master_rank = sub_rank;
        // Broadcast the master of the new communication to other processes
        MPI_Bcast(&master_rank, 1, MPI_INT, rank_counter, active_com);
        rank_counter +=1;
        if(membership==1){
            MPI_Bcast(&color_array[i*p_vertex_count_init], p_vertex_count_init, MPI_INT, master_rank, sub_comm);
            process_array[i]=0;
        }
        // MPI_Barrier(active_com);
        // MPI_Comm_free(&sub_comm);
    }
}

