# ParallelProcessing
Working through parallel processing concepts. To run the examples just CD into the Lab# directory for example from the root of this folder. 

```
cd Lab1
```
The instructions to setup and run the code will be provided as part of the readme here.

# Lab1

# Lab2

## Windows with WSL2 Setup
1. Install WSL2
2. Choose your flavour of Linux Code Tested on Ubuntu (20.04 DATE:2023-01-01)
3. Run Install-OpenCV Script
4. Install OpenMPI
5. Install X Server on Win10 or Win11 to Display GUI related things or comment it out in Lab2_part_3.cpp (imshow, imread, waitkey etc)
7. Cd ParallelProcessing/Lab2
8. cmake.
9. make 
10. Running the Samples ex: mpirun -np 4 p3 M_cali.jpg 1 4 out.jpg
11. Running mpi code format ex: mpirun -np [number of processors] [program name] [Path to image to perform the image processing] [Image processing operation number 1-4] [output filename with an image extension]

## Linux Tested on Ubuntu (20.04 DATE:2023-01-01)
1. Run Install-OpenCv Script
2. Install X
3. Clone Repo
4. Run Install-OpenCV Script
5. Install OpenMPI
6. cmake.
7. make 
8. Running the Samples ex: mpirun -np 4 p3 M_cali.jpg 1 4 out.jpg
9. Running mpi code format ex: mpirun -np [number of processors] [program name] [Path to image to perform the image processing] [Image processing operation number 1-4] [output filename with an image extension]
