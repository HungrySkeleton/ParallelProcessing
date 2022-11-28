all:	Lab1A	Lab1B	Lab1C
Lab1A:	problem1a.c
	mpicc problem1a.c -o p1a.out
Lab1B:	problem1b.c
	mpicc problem1b.c -o p1b.out
Lab1C:	problem1c.c
	mpicc problem1c.c -o parallel_p1c.out
	gcc problem1c.c -o serial_p1c.out
Lab2: problem2a_blocking.c	problem2a_immediate.c
	mpicc problem2a_blocking.c -o p2a_blocking.out
	mpicc problem2a_immediate.c -o p2a_immediate.out
clean:
	-rm *.out