#pragma once
#include <stdint.h>
#include <mpi.h>
#include "ref.h"

MPI_Comm scheduler_get_active_comm(int rank, int size, uint32_t* operating_processes_array);
void scheduler_broadcast(
    int rank,
    int size,
    uint32_t* process_array,
    uint32_t* operating_processes_array,
    MPI_Comm active_com,
    uint32_t p_vertex_count_init,
    uint32_t* color_array
);
