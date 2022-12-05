#include <stdio.h>
#include <mpi.h>
#include <math.h>


void generate_randoms();
void subvec();

int main(int argc, char** argv)
{
    int rank = 0;
    int nproc = 1;
    int namelength;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init (&argc, &argv); // Initialize MPI
    MPI_Comm_size (MPI_COMM_WORLD, &nproc); // Get the number of processors
    MPI_Comm_rank (MPI_COMM_WORLD, &rank); // Get my rank
    MPI_Get_processor_name(processor_name, &namelength);// Get the name of the processor and length
    
    return 0;
}

void generate_randoms()
{
    for(int i = 0; i < )
   int r = rand();
}