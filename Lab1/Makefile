all:	Lab1A	Lab1B	Lab1C	Lab2A	Lab2B
Lab1A:	problem1a.cpp
	mpicxx problem1a.cpp -o p1a.out
Lab1B:	problem1b.cpp
	mpicxx problem1b.cpp -o p1b.out
Lab1C:	problem1c.cpp
	mpicxx problem1c.cpp -DHAVEMPI -o parallel_p1c.out
	g++ problem1c.cpp -o serial_p1c.out
Lab2A: problem2a_blocking.cpp	problem2a_immediate.cpp
	mpicxx problem2a_blocking.cpp -o p2a_blocking.out
	mpicxx problem2a_immediate.cpp -o p2a_immediate.out
Lab2B: problem2b.cpp
	mpicxx problem2b.cpp -o p2b.out
clean:
	-rm *.out
	-rm *.txt