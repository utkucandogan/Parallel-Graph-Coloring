#pragma once

#include <cstdint>
#include <cstring>
#include <vector>
#include <stdexcept>
#include <immintrin.h>
#include <iostream>
class Neighbors;

class Adjacency
{
public:
    using index_t = uint32_t;

private:
    using storage_t = uint32_t;

    storage_t** m_data;
    index_t m_count;
    size_t m_rowSize;

    static constexpr size_t INDEX_T_BYTES = sizeof(index_t);
    static constexpr size_t STORAGE_T_BYTES = sizeof(storage_t);
    static constexpr size_t STORAGE_T_BITS  = sizeof(storage_t) * 8;


    Adjacency(index_t count,index_t maximum_number_elements);

public:
    ~Adjacency();
	storage_t* get_row(index_t row_number)const;
    index_t count() const;

    static Adjacency create(std::string filename);
};
