#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
    
    int rank = 0;
    int nproc = 1;
    int namelength;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Status status;
    MPI_Request request;

    MPI_Init (&argc, &argv); // Initialize MPI
    MPI_Comm_size (MPI_COMM_WORLD, &nproc); // Get the number of processors
    MPI_Comm_rank (MPI_COMM_WORLD, &rank); // Get my rank
    MPI_Get_processor_name(processor_name, &namelength);// Get the name of the processor and length

    if(rank==0)
    {
        char message_buffer[MPI_MAX_PROCESSOR_NAME];
        
        std::cout << "Number of processors is " << nproc << std::endl;
        std::cout << "Node " << rank <<" is processor " << processor_name << std::endl;

        for(int irank = 1; irank < nproc; irank++)
        {
            //Receive name from processor with rank = irank - store in "message_queue"
            MPI_Irecv(&message_buffer, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, irank, 99, MPI_COMM_WORLD, &request);
            MPI_Wait (&request, &status);
            std::cout << "Node " << rank << " receiving message:"  << message_buffer  << " from Node" << irank << std::endl;
        }
    }
    else
    {
        std::cout << "Node " << rank << " sending:"  << processor_name  << " to Node" << 0 << std::endl;
        MPI_Isend(&processor_name, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, 0, 99, MPI_COMM_WORLD, &request); // Send processor_n ame to processor 0.
        MPI_Wait(&request, MPI_STATUS_IGNORE) ;// Wait for MPI_Isend to complete before progressing further
    }


    MPI_Finalize();
    return 0;
}