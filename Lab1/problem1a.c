#include <stdio.h>

int main(int argc, char** argv) 
{
    int rank = 0;
    int nproc = 1;

    // Print off a hello world message
    printf("Hello, from processor rank %d of %d\n", rank, nproc);

    return 0;
}