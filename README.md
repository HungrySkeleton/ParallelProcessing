# ParallelProcessing
Working through parallel processing concepts. To run the examples just CD into the Lab# directory for example from the root of this folder. 

```
cd Lab1
```
The instructions to setup and run the code will be provided as part of the readme here.

# Lab1
1. Install OpenMPI
```
shell$ gunzip -c openmpi-4.1.4.tar.gz | tar xf -
shell$ cd openmpi-4.1.4
shell$ ./configure --prefix=/usr/local
<...lots of output...>
shell$ make -j8 all install
```
3. cmake .
4. make
5. Execute the files using mpirun or gcc when at the appropriate part
# Lab2

## Windows with WSL2 Setup
1. Install WSL2
2. Choose your flavour of Linux for WSL2 Code Tested on Ubuntu (20.04 DATE:2023-01-01)
3. Run Install-OpenCV Script
4. Install OpenMPI
5. Install X Server VCSXerv on Win10 or Win11 to Display GUI related things or comment out in Lab2_part_3.cpp (imshow, imread, waitkey etc)
6. Follow Setting up Xserv Guide
7. Cd ParallelProcessing/Lab2
8. cmake .
9. make 
10. Running the Samples ex: mpirun -np 4 p3 M_cali.jpg 1 4 out.jpg
11. Running mpi code format ex: mpirun -np [number of processors] [program name] [Path to image to perform the image processing] [Image processing operation number 1-4] [output filename with an image extension]

## Linux Tested on Ubuntu (20.04 DATE:2023-01-01)
1. Run Install-OpenCv Script
2. Clone Repo
3. Install OpenMPI
4. cmake .
5. make 
6. Running the Samples ex: mpirun -np 4 p3 M_cali.jpg 1 4 out.jpg
7. Running mpi code format ex: mpirun -np [number of processors] [program name] [Path to image to perform the image processing] [Image processing operation number 1-4] [output filename with an image extension]

## Sources:
- Installing MPI: https://www.open-mpi.org/faq/?category=building
- Installing OpenCV: https://github.com/milq/milq/blob/master/scripts/bash/install-opencv.sh (use milq's script)
- Setting up XServ: https://aalonso.dev/blog/how-to-use-gui-apps-in-wsl2-forwarding-x-server-cdj

