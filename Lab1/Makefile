all:	lab1a	lab1b	lab1c	lab2a	lab2b	lab3a	lab4a
lab1a:	problem1a.cpp
	mpicxx problem1a.cpp -o p1a.out
lab1b:	problem1b.cpp
	mpicxx problem1b.cpp -o p1b.out
lab1c:	problem1c.cpp
	mpicxx problem1c.cpp -DHAVEMPI -o parallel_p1c.out
	g++ problem1c.cpp -o serial_p1c.out
lab2a: problem2a_blocking.cpp	problem2a_immediate.cpp
	mpicxx problem2a_blocking.cpp -o p2a_blocking.out
	mpicxx problem2a_immediate.cpp -o p2a_immediate.out
lab2b:	problem2b.cpp
	mpicxx problem2b.cpp -o p2b.out
lab3a:	problem3.cpp
	mpicxx problem3.cpp -o p3a.out
lab4a:	problem4.cpp
	mpicxx problem4.cpp -o p4a.out
clean:
	-rm *.out
	-rm *.txt