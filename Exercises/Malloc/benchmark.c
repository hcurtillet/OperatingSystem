#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define ROUNDS 10
#define LOOP 100000

int main(){
    printf("Starting benchmark...\n");
	void * init = sbrk(0);
	void *current;
	printf("The initial top of the heap is %p\n", init);
	for(int i=0; i<ROUNDS; i++){
		for(int j=0; j<LOOP; j++){
			size_t size = (rand()%4000) +sizeof(int);
			int *memory;
			memory=malloc(size);
			if(memory == NULL){
				fprintf(stderr, "malloc failed\n");
				return (1);
			}
			free(memory);
		}
		current =sbrk(0);
		int allocated = (int)((current-init)/1024);
		printf("%d\n",i);
		printf("The current top of the heap is %p\n", current);
		printf("	increased by %d Kbyte\n", allocated);
	}
	return 0;
}
