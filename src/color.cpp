#include <iostream>

#include "color.hpp"

ColorArray::ColorArray(Adjacency::index_t vertexCount, color_t maxColor) : m_count(vertexCount), m_maxColor(maxColor)
{
    m_colors = new color_t[m_count];
    for (size_t i = 0 ; i < m_count ; ++i) {
        m_colors[i] = 0;
    }
}

ColorArray::~ColorArray()
{
    delete[] m_colors;
}

ColorArray::color_t& ColorArray::operator[](Adjacency::index_t index)
{
    if (index >= m_count) {
        throw std::out_of_range("Row index out of bounds");
    }
    return m_colors[index];
}

const ColorArray::color_t& ColorArray::operator[](Adjacency::index_t index) const
{
    if (index >= m_count) {
        throw std::out_of_range("Row index out of bounds");
    }
    return m_colors[index];
}

void ColorArray::print() const
{
    std::cout << "[Colors] ";

    for (Adjacency::index_t vertex = 0 ; vertex < m_count ; ++vertex) {
        std::cout << static_cast<int>(m_colors[vertex]) << ' ';
    }
    std::cout << "\n";
}

ForbiddenColors::ForbiddenColors(Adjacency::index_t colorCount) : m_count(colorCount + 1) // color zero is not valid but still exists for easy indexing
{
    m_vertices = new Adjacency::index_t[m_count];
    for (size_t i = 0 ; i < m_count ; ++i) {
        m_vertices[i] = Adjacency::END;
    }
}

ForbiddenColors::~ForbiddenColors()
{
    delete[] m_vertices;
}

Adjacency::index_t& ForbiddenColors::operator[](ColorArray::color_t color)
{
    if (color >= m_count) {
        throw std::out_of_range("Color out of bounds");
    }
    return m_vertices[color];
}

const Adjacency::index_t& ForbiddenColors::operator[](ColorArray::color_t color) const
{
    if (color >= m_count) {
        throw std::out_of_range("Color out of bounds");
    }
    return m_vertices[color];
}

bool check_colors(const Adjacency& adj, const ColorArray& colors)
{
    for (Adjacency::index_t vertex = 0 ; vertex < adj.count() ; ++vertex) {
        if (colors[vertex] == 0) return false; // Vertex is not colored
        for (auto neighbor = adj[vertex] ; *neighbor != Adjacency::END ; ++neighbor) {
            if (colors[vertex] == colors[*neighbor]) return false;
        }
    }

    return true;
}