# ParallelProcessing
Working through parallel processing concepts

# Lab1

# Lab2

## Windows Seting up Lab2
=> Install WSL2
=> Choose your flavour of Linux Code Tested on Ubuntu (20.04 DATE:2023-01-01)
=> Run Install-OpenCV Script
=> Install OpenMPI
=> Install X Server on Win10 or Win11 to Display GUI related things or comment it out in Lab2_part_3.cpp (imshow, imread, waitkey etc)
=> Cd ParallelProcessing/Lab2
=> cmake.
=> make 
=> Running the Samples ex: mpirun -np 4 p3 M_cali.jpg 1 4 out.jpg
=> Running mpi code format ex: mpirun -np [number of processors] [program name] [Path to image to perform the image processing] [Image processing operation number 1-4] [output filename with an image extension]

