#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "adjacency.h"
#include "parallel.h"

#define TEST_ITERATION_COUNT 1000
#define TEST_MEASUREMENTS_COUNT 1000

// Variable starting with p are process specific variables
int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    bool use_scheduler = false;
    // Select method
    if (strcmp(argv[1], "broadcaster") == 0) {
        use_scheduler = false;
    } else if (strcmp(argv[1], "scheduler") == 0) {
        use_scheduler = true;
    } else {
        fprintf(stderr, "Selected method is not valid!\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    if (rank == 0) {
        printf("Method: %s\n", argv[1]);
    }

    // read input
    uint32_t vertex_count;
    uint32_t max_degree;
    uint32_t* adjacency_array = NULL;
    if (rank == 0) {
        adjacency_array = read_input(argv[2], &vertex_count, &max_degree);
        // adjacency_array = generate_adjency_array(max_degree, vertex_count);
        printf("Size: %d, Vertex Count: %d, Max Degree: %d\n", size, vertex_count, max_degree);
    }

    size_t measurement;
    double measurement_array[TEST_MEASUREMENTS_COUNT];
    for(measurement = 0; measurement <TEST_MEASUREMENTS_COUNT; ++measurement){
        // measure start time
        MPI_Barrier(MPI_COMM_WORLD);
        double start_time = MPI_Wtime();

        size_t test_iterations;
        for (test_iterations = 0 ; test_iterations < TEST_ITERATION_COUNT ; ++test_iterations) {
            if (!parallel_color(rank, size, use_scheduler, vertex_count, max_degree, adjacency_array)) {
                fprintf(stderr, "Failure during the coloring algorithm!\n");
                break;
            }
        }

        double end_time = MPI_Wtime();
        double max_time;
        double elapsed_time = end_time - start_time;
        MPI_Reduce(&elapsed_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            //printf("Maximum elapsed time across all processes: %fms\n", max_time / test_iterations * 1000);
            measurement_array[measurement]= max_time/ test_iterations * 1000;
        }
    }

    // Here is the exit code that prints the timings into a file
    if (rank == 0) {
        // Free the array
        free(adjacency_array);

        // 1. Create a dynamic filename based on the array size
        // Assuming the array size won't exceed 100 digits for this example
        char filename[50];
        snprintf(filename, sizeof(filename), "V_%d_MD_%d_S_%d.txt", vertex_count,max_degree,size);

        // 2. Open the dynamically named file for writing
        FILE *file = fopen(filename, "w");
        if (file == NULL) { // Check if the file was opened successfully
            perror("Error opening file");
            return EXIT_FAILURE;
        }

        // 3. Write the array contents to the file
        for (int i = 0; i < TEST_MEASUREMENTS_COUNT; ++i) {
            // fprintf formats and writes data to the file
            // "%.2f" formats the double to two decimal places
            fprintf(file, "%f\n", measurement_array[i]);
        }
        // 4. Close the file to ensure data is flushed and resources are released
        if (fclose(file) != 0) {
            perror("Error closing file");
            return EXIT_FAILURE;
        }
        // Inform the user that the operation was successful
        printf("Array successfully written to %s with maximum precision.\n", filename);    
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
