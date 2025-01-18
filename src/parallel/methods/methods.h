#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <mpi.h>
#include "ref.h"

#define METHOD_INITIALIZE_RAW(NAME) \
bool NAME( \
    ref_type(uint32_t*) process_array, \
    ref_type(uint32_t*) operating_processes_array, \
    size_t size \
)

#define METHOD_FINALIZE_RAW(NAME) \
void NAME( \
    ref_type(uint32_t*) process_array, \
    ref_type(uint32_t*) operating_processes_array \
)

#define METHOD_PRECOLOR_RAW(NAME) \
MPI_Comm NAME(int rank, int size, uint32_t* operating_processes_array)

#define METHOD_POSTCOLOR_RAW(NAME) \
void NAME( \
    int rank, \
    int size, \
    uint32_t* process_array, \
    uint32_t* operating_processes_array, \
    MPI_Comm active_com, \
    uint32_t p_vertex_count_init, \
    uint32_t* color_array \
)

#define METHOD_INITIALIZE(NAME) METHOD_INITIALIZE_RAW(NAME##_initialize)
#define METHOD_FINALIZE(NAME) METHOD_FINALIZE_RAW(NAME##_finalize)
#define METHOD_PRECOLOR(NAME) METHOD_PRECOLOR_RAW(NAME##_precolor)
#define METHOD_POSTCOLOR(NAME) METHOD_POSTCOLOR_RAW(NAME##_postcolor)

#define METHOD_DECLARE_ALL(NAME) \
METHOD_INITIALIZE(NAME); \
METHOD_FINALIZE(NAME); \
METHOD_PRECOLOR(NAME); \
METHOD_POSTCOLOR(NAME)

METHOD_DECLARE_ALL(broadcaster);
METHOD_DECLARE_ALL(scheduler);

#define METHOD_FUNCTIONS_DEFINE() \
METHOD_INITIALIZE_RAW((*method_initialize)) = NULL; \
METHOD_FINALIZE_RAW((*method_finalize)) = NULL; \
METHOD_PRECOLOR_RAW((*method_precolor)) = NULL; \
METHOD_POSTCOLOR_RAW((*method_postcolor)) = NULL

#define METHOD_FUNCTIONS_ASSIGN(NAME) \
method_initialize = NAME##_initialize; \
method_finalize = NAME##_finalize; \
method_precolor = NAME##_precolor; \
method_postcolor = NAME##_postcolor
