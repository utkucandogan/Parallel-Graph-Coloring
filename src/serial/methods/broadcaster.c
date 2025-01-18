#include <stdio.h>
#include "methods.h"

METHOD_INITIALIZE(broadcaster)
{
    *process_array = NULL;
    *operating_processes_array = NULL;
    return true;
}

METHOD_FINALIZE(broadcaster)
{
    return;
}

METHOD_PRECOLOR(broadcaster)
{
    return 0;
}

METHOD_POSTCOLOR(broadcaster)
{
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
