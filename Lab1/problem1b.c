#include <stdio.h>
#include <mpi.h> //Use C−version of MPI, include mpi++.h for C++ bindings.

int main(int argc, char** argv) {
    
    int rank = 0;
    int nproc = 1;
    int namelength;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    MPI_Init(&argc, &argv); // Initialize the MPI environment
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);  // Get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get the rank of the process
    MPI_Get_processor_name(processor_name, &name_len);// Get the name of the processor and length

    printf("Hello my name is %s, rank %d out of %d processors\n", processor_name, rank, nproc); // Print off a hello world message
   
    MPI_Finalize(); // Finalize the MPI environment.

    return 0;
}