#include <iostream>
#include <mpi.h>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <numeric>
using namespace std;

//Prototypes for helper functions
void generate_randoms(vector<double>*, int);
vector<double> subvec(vector<double>, int, int);

void print(std::vector<double> const &input)
{
	for(int i =0; i < input.size(); i++)
	{
		std::cout << input.at(i) << ' ';
	}
}
void generate_randoms(vector<double>* list, int size)
{
	for(int i = 0; i < size; i++)
	{
		list->at(i) = rand();
	}
}

vector<double> subvec(vector<double> full_list, int rank, int n_values_proc)
{
	int start = rank*n_values_proc;
	vector<double> subvec(n_values_proc);
	//Calculate Partial SUM
	for(int i = start; i < full_list.size(); i++)
	{
		subvec.push_back(full_list.at(i));
	}
	return subvec;
}


int main(int argc, char** argv)
{
	int rank = 0;
	int nproc = 1;
	int namelength;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelength);

	int nvalues_per_proc = 0;
		
	double local_sum = 0;
	double total_sum_reduce = 0;
	double total_sum_manual = 0;
	
	if(rank == 0)
	{
		std::cout << "Enter the number of random values to be summed by each processor:" << std::endl;
		std::cin >> nvalues_per_proc;
		double time1, time2;
		MPI_Status status;
		srand(clock());
		vector<double> full_list (nproc * nvalues_per_proc); //Initialize Vector with zeros ( nproc X nvalues_per_proc )
		print(full_list);
		generate_randoms(&full_list, full_list.size()); //Generates a full list of values in the vector
		print(full_list);
				
		//Distribute the data to other process P0,P1,...,PN
		for(int iranks = 0; iranks < nproc; iranks++)
		{
			vector<double> subvector = subvec(full_list, iranks, nvalues_per_proc);
			MPI_Send(&subvector[0], nvalues_per_proc, MPI_DOUBLE,0,99,MPI_COMM_WORLD);	
		}
		vector<double> subVec0(nvalues_per_proc);
		MPI_Recv(&subVec0[0], nvalues_per_proc, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD, &status);	
		double partial_summ = accumulate(subVec0.begin(),subVec0.end(),0);
		//REMEMBER: the other processors don't know what nvalues_per_proc is

		MPI_Reduce(&local_sum, &total_sum_reduce, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); //Use MPI_REDUCE to contribute to the total sum on rank 0
		//Once all the processors have calculated their local sums, use MPI_REDUCE to generate a total sum on rank 0
		
		// Receive all partial sums from the other processors
		//Produce the total sum from the partial sums locally
		
		std::cout << "Partial Sum on Processor" << 0  << ":" << partial_summ << std::endl;
		total_sum_manual += partial_summ;

		//Compare the sum of MPI_reduce
		double partial_sum;
		for(int irank = 1; irank < nproc; irank++)
		{
			MPI_Recv(&partial_sum,1, MPI_DOUBLE, irank, 99, MPI_COMM_WORLD, &status);
			std::cout << "Partial Sum on Processor" << irank << ":" << partial_sum << std::endl;
			total_sum_manual += partial_sum;
		}
		
		std::cout << "Total Sum on all Processors (MPI_Reduce): " << total_sum_reduce << std::endl;
		std::cout << "Total Sum on all Processors (Manual sum): " << total_sum_manual << std::endl;
		std::cout << "Total time to compute: " << time2 - time1 << " seconds" << std::endl;
	}
	else
	{
		MPI_Status status;
		int count;
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status); //Probe for an incoming message from P0
	
		//When Probe returns, the status object has the size and other attributes of the incoing message. Get the message size
		MPI_Get_count(&status, MPI_INT, &count);
		
		vector<double> list (count); //Init local copy in P1..PN
		MPI_Recv(&list[0], count, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD, &status);	
		//Collect the subvector from rank 0
		
		//Calculate Partial SUM
		for(vector<double>::iterator ptr = list.begin(); ptr < list.end(); ptr++)
		{
			local_sum += *ptr;
		}
		
		MPI_Reduce(&local_sum, &total_sum_reduce, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); //Use MPI_REDUCE to contribute to the total sum on rank 0
		MPI_Send(&local_sum, 1, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD); //Send the partial sum to rank 0 for displaying and verifying	
		
	}
}