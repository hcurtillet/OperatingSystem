FLAGS= -std=c99 -Wall -Wextra -c -g

smalltest: smalltest.o green.o
	gcc -o smalltest.out smalltest.o green.o

smalltest.o: smalltest.c
	gcc -c smalltest.c

green.o: green.c
	gcc -c green.c

clean:
	rm -f *.o *.out