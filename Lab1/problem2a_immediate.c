#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    
    int rank = 0;
    int nproc = 1;
    int namelength;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    MPI_Status status;
    MPI_Request request;

    MPI_Init (&argc, &argv); // Initialize MPI
    MPI_Comm_size (MPI_COMM_WORLD, &nproc); // Get the number of processors
    MPI_Comm_rank (MPI_COMM_WORLD, &rank); // Get my rank
    MPI_Get_processor_name(processor_name, &name_len);// Get the name of the processor and length

    if(rank==0)
    {
        char message_buffer[MPI_MAX_PROCESSOR_NAME];
        
        printf("Number of processors is %d\n", nproc);
        printf("Node %d is processor %s\n",rank, processor_name);

        for(int irank = 1; irank < nproc; irank++)
        {
            //Receive name from processor with rank = irank - store in "message_queue"
            MPI_Irecv(&message_buffer, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, irank, 99, MPI_COMM_WORLD, &request);
            MPI_Wait (&request, &status);
            printf("Node %d receiving message \"%s\" from Node %d\n", rank, message_buffer, irank);
        }
    }
    else
    {
        printf("Node %d sending \"%s\" to Node %d\n", rank, processor_name, 0);
        MPI_Isend(&processor_name, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, 0, 99, MPI_COMM_WORLD, &request); // Send processor_n ame to processor 0.
        MPI_Wait(&request, MPI_STATUS_IGNORE) ;// Wait for MPI_Isend to complete before progressing further
    }


    MPI_Finalize();
    return 0;
}