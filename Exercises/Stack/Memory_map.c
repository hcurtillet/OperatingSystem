#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "Memory_map.h"

char global[] = "This is a global string";

int main(){
	int pid = getpid();
	unsigned long p = 0x1;

	//foo(&p);
back: 
	printf("Process id: %d\n", pid);
	printf("Global string: %p\n", &global);
	printf("p (0x%ld): %p\n", p, &p);
	printf("back: %p\n", &&back);

	long *heap = calloc(40, sizeof(unsigned long));
	printf("heap[2] = 0x%ld\n", heap[2]);
	printf("heap[1] = 0x%ld\n", heap[1]);
	printf("heap[0] = 0x%ld\n", heap[0]);
	printf("heap[-1] = 0x%ld\n", heap[-1]);
	printf("heap[-2] = 0x%ld\n", heap[-2]);
	free(heap);
	printf("The heap is free\n");
	printf("heap[2] = 0x%ld\n", heap[2]);
	printf("heap[1] = 0x%ld\n", heap[1]);
	printf("heap[0] = 0x%ld\n", heap[0]);
	printf("heap[-1] = 0x%ld\n", heap[-1]);
	printf("heap[-2] = 0x%ld\n", heap[-2]);	


	printf("/proc/%d/maps\n\n", pid);
	char command[50];
	sprintf(command, "cat /proc/%d/maps", pid);
	//system(command);
	// fgetc(stdin);
	return 0;
}

void zot (unsigned long *stop) {
	unsigned long r = 0x3 ;
	unsigned long * i ;
	for ( i = &r ; i <= stop ; i++){	
		printf("%p : 0x%ld\n", i, *i);
	}
}

void foo(unsigned long *stop){
	unsigned long q = 0x2;
	zot(stop);
}