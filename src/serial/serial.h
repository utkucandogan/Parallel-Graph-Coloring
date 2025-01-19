#pragma once
#include <stdint.h>
#include <stdbool.h>

bool serial_color(uint32_t vertex_count, uint32_t max_degree, uint32_t* adjacency_array);
void color_vertices_static(uint32_t* adjacency_array, uint32_t* color_array, uint32_t* forbidden_colors, uint32_t vertex_count, uint32_t max_degree);