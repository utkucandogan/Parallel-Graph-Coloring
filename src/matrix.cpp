#include <fstream>

#include "matrix.hpp"

Adjacency::Adjacency(Adjacency::index_t count) : m_count(count)
{
    m_rowSize = (count + STORAGE_T_BITS - 1) / STORAGE_T_BITS; // divide to ceiling


    size_t size = m_rowSize * m_count;
    m_data = new storage_t[size];
}

Adjacency::~Adjacency()
{
    delete[] m_data;
}

Adjacency::index_t Adjacency::count() const
{
    return m_count;
}

Neighbors Adjacency::operator[](Adjacency::index_t row) const
{
    if (row >= m_count) throw std::out_of_range("");
    return Neighbors(m_data + m_rowSize*row, m_rowSize);
}

Adjacency Adjacency::create(const char* filename)
{
    std::ifstream f(filename, std::ios::binary);
    if (!f.is_open()) {
        throw std::runtime_error("Couldn\'t open the file!");
    }

    // read the number of vertices
    storage_t data;
    f.read(reinterpret_cast<char*>(&data), STORAGE_T_BYTES);

    Adjacency adj((index_t) data);

    // iterate over the data
    for (size_t i = 0 ; i < adj.m_count * adj.m_rowSize ; ++i) {
        f.read(reinterpret_cast<char*>(adj.m_data + i), STORAGE_T_BYTES);
    }

    f.close();

    return adj;
}

Neighbors::Neighbors(Adjacency::storage_t* data, size_t size) : m_data(data), m_size(size) { }

std::vector<Adjacency::index_t> Neighbors::to_vector() const
{
    std::vector<Adjacency::index_t> neighbors;
    for (size_t i = 0 ; i < m_size ; ++i) {
        uint32_t data = m_data[i];
        while (data) {
            auto index = _bit_scan_forward(data);
            neighbors.push_back(Adjacency::STORAGE_T_BITS * i + index);
            data &= ~(1 << index);
        }
    }
    return neighbors;
}
