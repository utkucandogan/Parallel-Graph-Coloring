#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "adjacency.h"
#include "serial.h"

#define TEST_ITERATION_COUNT 100
#define TEST_MEASUREMENTS_COUNT 100

// Variable starting with p are process specific variables
int main(int argc, char* argv[])
{


    // read input
    uint32_t vertex_count;
    uint32_t max_degree;
    uint32_t* adjacency_array = NULL;
    adjacency_array = read_input(argv[1], &vertex_count, &max_degree);
    // adjacency_array = generate_adjency_array(max_degree, vertex_count);
    printf("Vertex Count: %d, Max Degree: %d\n", vertex_count, max_degree);


    struct timespec start, end;
    double elapsed_time;
    size_t measurement;
    double measurement_array[TEST_MEASUREMENTS_COUNT];
    for(measurement = 0; measurement <TEST_MEASUREMENTS_COUNT; ++measurement){
        // Get the starting time using CLOCK_MONOTONIC
        if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
            perror("clock_gettime");
            return 1;
        }

        size_t test_iterations;
        for (test_iterations = 0 ; test_iterations < TEST_ITERATION_COUNT ; ++test_iterations) {
            if (!serial_color( vertex_count, max_degree, adjacency_array)) {
                fprintf(stderr, "Failure during the coloring algorithm!\n");
                break;
            }
        }

        // Get the ending time
        if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
            perror("clock_gettime");
            return 1;
        }
        // Calculate elapsed time in miliseconds
        elapsed_time = (end.tv_sec - start.tv_sec) +
                (end.tv_nsec - start.tv_nsec) / 1e6;

        measurement_array[measurement]= elapsed_time/ test_iterations;
    }

        // Free the array
        free(adjacency_array);

        // 1. Create a dynamic filename based on the array size
        // Assuming the array size won't exceed 100 digits for this example
        char filename[50];
        snprintf(filename, sizeof(filename), "Serial_M_%dx%d.txt", vertex_count,max_degree);

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

    return EXIT_SUCCESS;
}
