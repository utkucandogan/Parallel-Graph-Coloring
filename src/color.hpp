#pragma once

#include <cstdint>
#include <stdexcept>
#include "adjacency.hpp"

class ColorArray
{
public:
    using color_t = uint8_t;

private:
    color_t* m_colors;
    Adjacency::index_t m_count;
    color_t m_maxColor;

public:
    ColorArray(Adjacency::index_t vertexCount, color_t maxColor);
    ~ColorArray();

    color_t max_color() const
    {
        return m_maxColor;
    }

    color_t& operator[](Adjacency::index_t index);
    const color_t& operator[](Adjacency::index_t index) const;

    void print() const;
};

class ForbiddenColors
{
private:
    Adjacency::index_t* m_vertices;
    ColorArray::color_t m_count;

public:
    ForbiddenColors(Adjacency::index_t colorCount);
    ~ForbiddenColors();

    Adjacency::index_t& operator[](ColorArray::color_t color);
    const Adjacency::index_t& operator[](ColorArray::color_t color) const;
};

bool check_colors(const Adjacency& adj, const ColorArray& colors);
