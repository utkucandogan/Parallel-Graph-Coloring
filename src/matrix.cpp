#include <fstream>

#include "matrix.hpp"

Adjacency::Adjacency(Adjacency::index_t count) : m_count(count)
{
    
    m_data = new storage_t* [m_count];
	for(int i = 0; i < m_count; i++){
		m_data[i] = new storage_t [m_count]
		m_data[i][0] = 0;
	}
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
	storage_t matrix_size = data;

    Adjacency adj((index_t) matrix_size);

    // iterate over the data
	storage_t col_counter =0;
	storage_t row = 0,old_row = 0;
	storage_t col = 0;
	while (f.read(reinterpret_cast<char*>(&data), sizeof(STORAGE_T_BYTES))) {
		old_row = row;
		ol_col = col;
		row = data / matrix_size;        // Extract row (0-based)
        col = data % matrix_size;        // Extract column (0-based)
		// While finding neighbors every row will be read until 0 is reached
		// This will save resources
		if(old_row != row){
			adj.m_data[old_row][col_counter]=0;
			col_counter = 0;
		}
		adj.m_data[row][col_counter]=col+1;
		col_counter += 1;
    }
	// after reacing the end of the file, the last 0 must be added to the end of the last written row
	adj.m_data[row][col_counter]=0;

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
