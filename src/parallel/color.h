#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "ref.h"

void color_vertices_dynamic(
    uint32_t* p_adjacency_array,
    uint32_t* color_array,
    uint32_t* forbidden_colors,
    uint32_t* p_collision_array,
    uint32_t* process_array,
    uint32_t p_vertex_start_index,
    uint32_t vertex_count,
    uint32_t p_vertex_count_init,
    uint32_t max_degree
);

bool check_collisions(
    ref_type(uint32_t*) p_collision_array,
    ref_type(uint32_t) p_vertex_count,
    uint32_t* p_adjacency_array,
    uint32_t* color_array,
    uint32_t* operating_processes_array,
    uint32_t rank,
    uint32_t p_vertex_start_index,
    uint32_t max_degree
);

bool allocate_and_initialize(
    ref_type(uint32_t*) p_forbidden_colors,
    ref_type(uint32_t*) p_collision_array,
    ref_type(uint32_t*) p_adjacency_array,
    ref_type(uint32_t*) color_array,
    ref_type(uint32_t*) process_array,
    ref_type(uint32_t*) operating_processes_array,
    size_t size,
    size_t max_degree,
    size_t p_vertex_count,
    size_t vertex_count
);
