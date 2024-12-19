#include "matrix.hpp"

int main(int argc, char* argv[])
{
	std::string filename = argv[1];
    std::cout << "Hello world" << std::endl;

    auto adj = Adjacency::create(filename);
    for (size_t i = 0 ; i < adj.count() ; ++i) {
		uint32_t* row_data =adj.get_row(i);
		int row_counter = 0;
		while(1){
		if(row_data[row_counter] == 0)
			break;
        std::cout << "Row_Number:" << i + 1 << "Neighbor:" << row_data[row_counter] << "\n";
		row_counter += 1;
		}
    }
}
