#include "scheduler.h"

MPI_Comm scheduler_get_active_comm(int rank, int size, uint32_t* operating_processes_array)
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
    MPI_Comm active_comm;
    MPI_Comm_split(MPI_COMM_WORLD, active_process, rank, &active_comm);


    int active_rank = 0;
    for(int i = 0 ; i < size ; ++i){
        if(i == rank) break;
        if(operating_processes_array[i] == 1) {
            active_rank += 1;
        }
    }

    return active_comm;
}

void scheduler_broadcast(
    int rank,
    int size,
    uint32_t* process_array,
    uint32_t* operating_processes_array,
    MPI_Comm active_com,
    uint32_t p_vertex_count_init,
    uint32_t* color_array
)
{
    int rank_counter = 0;
    // Every active process will send its part of the color array to the needing processes
    for (int i = 0; i < size; i++) {
        if(operating_processes_array[i] == 0){
            continue;
        }
        int membership = ((process_array[i] == 1) || rank == i ) ? 1 : MPI_UNDEFINED;
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

