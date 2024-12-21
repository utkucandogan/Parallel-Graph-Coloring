#include <fstream>
#include <iostream>

#include "adjacency.hpp"
#include "log.hpp"

// Degree is the degree of the graph, count is the number of vertices
Adjacency::Adjacency(Adjacency::index_t count, Adjacency::index_t degree) : m_count(count), m_degree(degree)
{
    // Create a list for each vertex that contains neighbor vertex indices
    m_data = new index_t* [m_count];
    for(size_t i = 0 ; i < m_count ; ++i) {
        m_data[i] = new index_t [m_degree + 1];
        // Initially the neighbor list is empty
        m_data[i][0] = END;
    }
}

Adjacency::~Adjacency()
{
    // Delete each vertex neighbor list
    for(size_t i = 0 ; i < m_count ; ++i) {
        delete[] m_data[i];
    }
    // Delete the list of vertices
    delete[] m_data;
}

const Adjacency::index_t* Adjacency::operator[](index_t vertex) const
{
    if (vertex > m_count) {
        throw std::out_of_range("Row index out of bounds");
    }
    return m_data[vertex]; // Return the pointer to the specified row
}

void Adjacency::print() const
{
    std::cout << "[Matrix] Vertex count: " << m_count << ", Maximum degree: " << m_degree << "\n";

    for (Adjacency::index_t vertex = 0 ; vertex < m_count ; ++vertex) {
        std::cout << "\tVertex " << vertex << ": ";
        for (auto neighbor = m_data[vertex] ; *neighbor != Adjacency::END ; ++neighbor) {
            std::cout << *neighbor << ' ';
        }
        std::cout << "\n";
    }
}

Adjacency Adjacency::create(const char* filename)
{
    std::ifstream f(filename, std::ios::binary);
    if (!f.is_open()) {
        throw std::runtime_error("Couldn\'t open the file!");
    }

    // Read the number of vertices and max array size
    index_t vertexCount;
    index_t maxDegree;

    f.read(reinterpret_cast<char*>(&vertexCount), sizeof(index_t));
    f.read(reinterpret_cast<char*>(&maxDegree), sizeof(index_t));

    log("Vertex count: %d\n", vertexCount);
    log("Max degree: %d\n", maxDegree);

    Adjacency adj(vertexCount, maxDegree);

    // Iterate over the data
    index_t data;

    index_t col_counter = 0;
    index_t row = 0, old_row = 0;
    index_t col = 0;
    while (f.read(reinterpret_cast<char*>(&data), sizeof(index_t))) {
        old_row = row;
        row = data / vertexCount;        // Extract row (0-based)
        col = data % vertexCount;        // Extract column (0-based)
        // While finding neighbors every row will be read until 0 is reached
        // This will save resources
        log("Row number: %d, Column counter: %d\n", col, col_counter);
        if(old_row != row){
            adj.m_data[old_row][col_counter] = END;
            col_counter = 0;
        }
        adj.m_data[row][col_counter] = col;
        col_counter += 1;
    }
    // After reaching the end of the file, the last 0 must be added to the end of the last written row
    adj.m_data[row][col_counter] = END;

    f.close();

    return adj;
}
