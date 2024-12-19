#include <fstream>

#include "matrix.hpp"

Adjacency::Adjacency(Adjacency::index_t count,Adjacency::index_t maximum_number_elements) : m_count(count)
{
    
    m_data = new storage_t* [m_count];
	for(int i = 0; i < m_count; i++){
		m_data[i] = new storage_t [maximum_number_elements];
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


Adjacency Adjacency::create(std::string filename)
{
    std::ifstream f(filename, std::ios::binary);
    if (!f.is_open()) {
        throw std::runtime_error("Couldn\'t open the file!");
    }

    // read the number of vertices and max array size
	
    storage_t data;
    f.read(reinterpret_cast<char*>(&data), STORAGE_T_BYTES);
	storage_t matrix_size = data;
	std::cout << "Row_size:" << matrix_size << "\n";
	f.read(reinterpret_cast<char*>(&data), STORAGE_T_BYTES);
	storage_t maximum_number_elements = data;
    Adjacency adj((index_t) matrix_size,(index_t) maximum_number_elements);


    // iterate over the data
	storage_t col_counter =0;
	storage_t row = 0,old_row = 0;
	storage_t col = 0;
	while (f.read(reinterpret_cast<char*>(&data),STORAGE_T_BYTES)) {
		old_row = row;
		row = data / matrix_size;        // Extract row (0-based)
        col = data % matrix_size;        // Extract column (0-based)
		// While finding neighbors every row will be read until 0 is reached
		// This will save resources
		std::cout << "Row number :" << col << "col countr" <<col_counter<<"\n";
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

Adjacency::storage_t* Adjacency::get_row(index_t row_number)const
{
	
	if (row_number >= m_count || row_number < 0) {
		throw std::out_of_range("Row index out of bounds");
	}
	return m_data[row_number]; // Return the pointer to the specified row
}

