#include<string.h>
#include<stdio.h>
#include<mpi.h>

int main(int argc, char** argv)
{
    int rank = 0;
    int nproc = 1;
    int namelength;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    MPI_Init (&argc, &argv); // Initialize MPI
    MPI_Comm_size (MPI_COMM_WORLD, &nproc); // Get the number of processors
    MPI_Comm_rank (MPI_COMM_WORLD, &rank); // Get my rank
    MPI_Get_processor_name(processor_name, &name_len);// Get the name of the processor and length

    FILE *file;
    char buffer[MPI_MAX_PROCESSOR_NAME]; // The filename buffer.
    char data[MPI_MAX_PROCESSOR_NAME * 2]; //Data Buffer has to be twice the size of MPI_MAX_PROCESSOR Name since the constant is defind as 128 making sure if we copy it over it doesn't overflow
    
    // Put "file" then k then ".txt" in to filename.
    snprintf(buffer, sizeof(char) * MPI_MAX_PROCESSOR_NAME, "Rank%iOutofProcessors%i.txt", rank, nproc);
    snprintf(data, sizeof(char) * MPI_MAX_PROCESSOR_NAME * 2, "My name is %s and I am Rank %i Out of %i Processors",processor_name, rank, nproc);

    // here we get some data into variable data

    file = fopen(buffer, "wb");
    fwrite (data, 1, strlen(data) , file);
    fclose(file);
    
    MPI_Finalize();
    return 0;
}