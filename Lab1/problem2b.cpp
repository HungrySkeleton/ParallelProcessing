#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <mpi.h>

int main(int argc, char** argv)
{
    int rank = 0;
    int nproc = 1;
    int namelength;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    std::ostringstream oss;
    std::ofstream out_to_file;
    std::string s;

    MPI_Init (&argc, &argv); // Initialize MPI
    MPI_Comm_size (MPI_COMM_WORLD, &nproc); // Get the number of processors
    MPI_Comm_rank (MPI_COMM_WORLD, &rank); // Get my rank
    MPI_Get_processor_name(processor_name, &namelength);// Get the name of the processor and length

    oss << "Rank" << rank <<"OutofProcessors" << nproc <<".txt"; //Create File Name
    s = oss.str(); //Convert OSS to string

    out_to_file.open(s);
    out_to_file << "My name is " << processor_name << " and I am processor " << rank << " of " << nproc << std::endl;
    out_to_file.close();

    MPI_Finalize();
    return 0;
}