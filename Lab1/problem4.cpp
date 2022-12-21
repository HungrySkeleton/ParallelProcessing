#include <mpi.h>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <math.h>

using namespace std;

int main(int argc, char**argv)
{
	int rank = 0;
	int nproc = 1;
	int root_rank = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc); //Get the number of processors
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get my rank
	
	switch(rank)
	{
		case 0:
		{
			int my_value;
			vector<int> buffer = {100, 0, 101, 102, 0, 0, 103}; 
			vector<int> counts = {1, 2, 1}; // How much each processor should receive
			vector<int> displacements = {0, 2, 6}; //Starting point for the contiguous memory of each processor
			std::cout << "Values in the buffer of root process:" << std::endl;
			for(auto& v: buffer)
			{
				std::cout << v << " ";
			}
			std::cout << std::endl;
			MPI_Scatterv(&buffer[0], &counts[0], &displacements[0], MPI_INT,&my_value, 1, MPI_INT, root_rank, MPI_COMM_WORLD);
			std::cout << "Process" << rank << " received value " << my_value << std::endl; 
		}	
		case 1:
		{
			vector<int> my_values(2);
			MPI_Scatterv(NULL, NULL, NULL, MPI_INT, &my_values[0], 2, MPI_INT, root_rank, MPI_COMM_WORLD);
			std::cout << "Process" << rank << " received value " << my_values[0] << " and " << my_values[1]  << std::endl; 

		}
		case 2:
		{
			int my_values;
			MPI_Scatterv(NULL, NULL, NULL, MPI_INT, &my_values, 1, MPI_INT, root_rank, MPI_COMM_WORLD);
			std::cout << "Process" << rank << " received value " << my_values << std::endl; 
		}
	}
	MPI_Finalize();
	return 0;
}
