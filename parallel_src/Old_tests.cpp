#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int All_gather_test(int argc, char* argv[])
{
    int rank, size;

    // Initialize MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // This example is designed for exactly 4 processes
    if (size != 4) {
        if (rank == 0) {
            printf("This program is designed to run with exactly 4 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Each process will send 4 integers
    // Example: process 0 -> [1, 2, 3, 4]
    //          process 1 -> [5, 6, 7, 8], etc.
    int send_values[4];
    for (int i = 0; i < 4; i++) {
        send_values[i] = rank * 4 + (i + 1);
    }

    // Buffer to receive data from all processes (4 processes * 4 integers = 16)
    int recv_values[16];

    // Perform the Allgather operation
    // Each process sends 4 integers and receives 4 integers from each of the 4 processes.
    MPI_Allgather(
        send_values,  // send buffer (address of the data to send)
        4,            // number of elements to send
        MPI_INT,      // type of elements to send
        recv_values,  // receive buffer (where data will be gathered)
        4,            // number of elements received from each process
        MPI_INT,      // type of elements to receive
        MPI_COMM_WORLD
    );

    // Print the gathered results
    // Each process will now have 16 integers in recv_values: 4 from each process
    printf("Process %d received: ", rank);
    for (int i = 0; i < 16; i++) {
        printf("%d ", recv_values[i]);
    }
    printf("\n");

    // Finalize the MPI environment
    MPI_Finalize();

    return 0;
}

int All_Gather_2D_test(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // For simplicity, let's assume 4 processes
    if (size != 4) {
        if (rank == 0) {
            printf("Please run this with 4 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Each process has a 2x2 local array
    // We'll fill it with rank-specific values
    int local_data[2][2];
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            local_data[i][j] = (rank + 1) * 10 + (i * 2 + j);
        }
    }

    // We'll gather into a global 2D array [8 x 2], conceptually
    // In practice, we must allocate it as a 1D array with 16 elements 
    // or as an int global_data[8][2]. For clarity, let's do:
    int global_data[8][2];  // Only root usage is typical in Gather, 
    // but Allgather means EVERY process receives the full data.

// Each process sends 2*2 = 4 integers;
// The total data in global_data is 4 processes * 4 ints = 16 ints.
// As a 2D array [8][2], that still is 16 ints in row-major order.

    MPI_Allgather(
        &local_data[0][0],  // start address of local 2D array
        4,                  // 2*2 elements per process
        MPI_INT,
        &global_data[0][0], // start address of global 2D array
        4,
        MPI_INT,
        MPI_COMM_WORLD
    );

    // Print results on each rank
    printf("Rank %d received global_data:\n", rank);
    for (int i = 0; i < 8; i++) {
        printf("  ");
        for (int j = 0; j < 2; j++) {
            printf("%d ", global_data[i][j]);
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}


int Sync_buffer(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // For this demo, let's pick a simple N, say 16, and require 4 processes.
    // In practice, you can set N however you like, or pass it in via argv, etc.
    int N = 16;
    if (size != 4) {
        if (rank == 0) {
            printf("Please run this with 4 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // local_vertex_count is how many elements each rank is responsible for.
    // For example, rank 0 modifies indices [0..3], rank 1 modifies [4..7], etc.
    int local_vertex_count = N / size;  // assume N is divisible by size

    // Allocate the full array on each process, initialize to 0.
    int* global_array = new int[N];
    for (int i = 0; i < N; i++) {
        global_array[i] = 0;
    }

    // Each rank modifies only its portion of the array:
    // e.g. rank 0 modifies global_array[0..3],
    //      rank 1 modifies global_array[4..7], etc.
    int start = rank * local_vertex_count;
    int end = start + local_vertex_count;
    for (int i = start; i < end; i++) {
        // Example: write a recognizable pattern
        // to confirm each rank's slice
        global_array[i] = 100 * rank + (i - start);
    }

    // Now we want each process to see the fully-updated array of length N.
    // We'll use MPI_Allgather IN PLACE. 
    //
    // The in-place usage means:
    //   - sendbuf = MPI_IN_PLACE
    //   - sendcount / sendtype are ignored
    //   - The receive buffer must be large enough for size * recvcount elements
    //
    // In Allgather, the data from rank r will appear in the r-th block
    // of 'recvcount' elements in the receive buffer. Here, recvcount=local_vertex_count,
    // so the portion [r*local_vertex_count .. r*local_vertex_count + local_vertex_count - 1] belongs to rank r.
    //
    // This works perfectly if each rank has already placed its updated
    // data in the correct region (which we did).

    MPI_Allgather(
        MPI_IN_PLACE, // "in-place" send buffer
        0,            // ignored
        MPI_DATATYPE_NULL, // ignored
        global_array, // receive buffer (must have N=local_vertex_count*size capacity)
        local_vertex_count,      // how many elements each rank contributes
        MPI_INT,
        MPI_COMM_WORLD
    );

    // After this Allgather, every process's global_array
    // should contain the combined result from all ranks.
    // Let's print them out to check:
    printf("Rank %d: final global_array = ", rank);
    for (int i = 0; i < N; i++) {
        printf("%d ", global_array[i]);
    }
    printf("\n");

    free(global_array);
    MPI_Finalize();
    return 0;
}

int Scatter_test(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // For simplicity, assume N=16 and size=4.
    // In practice, you can set N as needed.
    int N = 16;
    if (size != 4) {
        if (rank == 0) {
            printf("Please run this with 4 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // local_vertex_count = 16 / 4 = 4
    int local_vertex_count = N / size;

    // Allocate buffer for each rank to store its local chunk
    int* local_array = (int*)malloc(local_vertex_count * sizeof(int));

    // Rank 0 allocates and initializes the big array
    int* big_array = NULL;
    if (rank == 0) {
        big_array = (int*)malloc(N * sizeof(int));
        // Example initialization
        // big_array = [0, 1, 2, 3, 4, 5, 6, 7, ..., 15]
        for (int i = 0; i < N; i++) {
            big_array[i] = i;
        }
    }

    // Scatter the big array to all processes
    //  - On rank 0, MPI_Scatter will send the appropriate chunk to each rank
    //  - On other ranks, MPI_Scatter will receive that chunk
    // The signature:
    //   MPI_Scatter(sendbuf, sendcount, sendtype,
    //               recvbuf, recvcount, recvtype,
    //               root, comm)
    MPI_Scatter(
        big_array,     // send buffer (only significant at root=0)
        local_vertex_count,       // how many elements to send to each rank
        MPI_INT,
        local_array,   // receive buffer on each rank
        local_vertex_count,
        MPI_INT,
        0,             // root (which holds the big array)
        MPI_COMM_WORLD
    );

    // After scatter, each rank has its own slice in local_array
    // For example, rank 0 has elements [0,1,2,3],
    // rank 1 has [4,5,6,7], rank 2 has [8,9,10,11], and rank 3 has [12,13,14,15].

    // Now it's safe to free big_array on rank 0
    if (rank == 0) {
        free(big_array);
        big_array = NULL;
    }

    // Print out what each rank received
    printf("Rank %d received: ", rank);
    for (int i = 0; i < local_vertex_count; i++) {
        printf("%d ", local_array[i]);
    }
    printf("\n");

    // Free memory
    free(local_array);
    if (rank == 0) {
        free(big_array);
    }

    MPI_Finalize();
    return 0;
}
