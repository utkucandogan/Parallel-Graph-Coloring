#pragma once
#include <stdint.h>
#include <stdbool.h>

uint32_t* generate_adjacency_array(uint32_t max_degree, uint32_t vertex_count);
void print_adjacency_array(uint32_t* matrix, uint32_t vertex_count, uint32_t max_degree);
uint32_t* read_input(const char* filename, uint32_t* vertex_count, uint32_t* max_degree);
