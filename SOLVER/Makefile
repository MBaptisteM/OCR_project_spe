solver : Solver.o
	gcc Solver.o -o solver -std=c99 -Wall -Wextra -lm

Solver.o : Solver.c
	gcc -c Solver.c -std=c99 -Wall -Wextra

clean:
	rm -f Solver.o
	rm -f solver

.PHONY: all clean