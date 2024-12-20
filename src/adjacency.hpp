#pragma once

#include <cstdint>

class Adjacency
{
public:
    using index_t = uint32_t;
    static constexpr index_t END = -1;

private:
    index_t** m_data;
    index_t m_count;
    index_t m_degree;

    Adjacency(index_t count, index_t degree);

public:
    ~Adjacency();
    index_t count() const
    {
        return m_count;
    }
    index_t degree() const{
        return m_degree;
    }

    const index_t* operator[](index_t vertex) const;
    void print() const;

    static Adjacency create(const char* filename);
};
