#pragma once

#include <cstdint>
#include <cstring>
#include <vector>
#include <stdexcept>
#include <immintrin.h>

class Neighbors;

class Adjacency
{
public:
    using index_t = uint32_t;

private:
    using storage_t = uint32_t;

    storage_t* m_data;
    index_t m_count;
    size_t m_rowSize;

    static constexpr size_t STORAGE_T_BYTES = sizeof(storage_t);
    static constexpr size_t STORAGE_T_BITS  = sizeof(storage_t) * 8;

    friend Neighbors;

    Adjacency(index_t count);

public:
    ~Adjacency();

    index_t count() const;
    Neighbors operator[](index_t row) const;

    static Adjacency create(const char* filename);
};

class Neighbors
{
private:
    Adjacency::storage_t* m_data;
    size_t m_size;

    friend Adjacency;

    Neighbors(Adjacency::storage_t* data, size_t size);

public:
    std::vector<Adjacency::index_t> to_vector() const;
};
