FLAGS= -std=c99 -Wall -Wextra -c -g

test: test.o dlmall.o
	gcc -o test.out test.o dlmall.o
benchmark: benchmark.o dlmall.o
	gcc -o benchmark.out benchmark.o dlmall.o
dlmall.o: dlmall.c
	gcc -c dlmall.c

test.o: test.c
	gcc -c test.c

benchmark.o: benchmark.c
	gcc -c benchmark.c
clean:
	rm -f *.o *.out