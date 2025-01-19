#pragma once
#include <stdint.h>
#include <stdbool.h>

bool parallel_color(int rank, int size, bool use_scheduler, uint32_t vertex_count, uint32_t max_degree, uint32_t* adjacency_array);
