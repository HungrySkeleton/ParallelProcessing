all:	Lab1A	Lab1B	Lab1C
Lab1A:	problem1a.cpp
	mpicxx problem1a.cpp -o p1a.out
Lab1B:	problem1b.cpp
	mpicxx problem1b.cpp -o p1b.out
Lab1C:	problem1c.cpp
	mpicxx problem1c.cpp -o parallel_p1c.out
	g++ problem1c.cpp -o serial_p1c.out
Lab2A: problem2a_blocking.c	problem2a_immediate.c
	mpicc problem2a_blocking.c -o p2a_blocking.out
	mpicc problem2a_immediate.c -o p2a_immediate.out
Lab2B: problem2b.c
	mpicc problem2b.c -o p2b.out
clean:
	-rm *.out
	-rm *.txt