#include <iostream>

int main(int argc, char** argv) 
{
    int rank = 0;
    int nproc = 1;

    // Print off a hello world message
    std::cout << "Hello, from processor rank " << rank << " of " << nproc << std::endl; 

    return 0;
}