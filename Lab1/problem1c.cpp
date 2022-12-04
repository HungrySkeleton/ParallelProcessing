#include <iostream>
#ifdef HAVEMPI
#include <mpi++.h> //Use C−version of MPI, include mpi++.h for C++ bindings.
#endif

//Purpose: To modify problem1b.c so it's compatible compiling with gcc and mpicc for serial and parallel code
int main(int argc, char** argv) {
    
    int rank = 0;
    int nproc = 1;

    #ifdef HAVEMPI
    int namelength;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    MPI_Init(&argc, &argv); // Initialize the MPI environment
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);  // Get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get the rank of the process
    MPI_Get_processor_name(processor_name, &name_len);// Get the name of the processor and length

    std::cout << "Hello, my name is" << processor_name <<", rank " << rank << " out of " << nproc << " processors" << std::endl;
   
    MPI_Finalize(); // Finalize the MPI environment.
    #endif

    #ifndef HAVEMPI
    const char * processor_name = " default";
    if (rank == 0){
        std::cout << "Hello, my name is" << processor_name <<", rank " << rank << " out of " << nproc << " processors" << std::endl;
    }
    #endif

    return 0;
}