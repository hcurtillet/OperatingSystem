#include <stdio.h>
#include <stdlib.h>
#define MEMORY 100

int memory[MEMORY];

typedef struct array {
	int size;
	int *segment;
	struct array* next;
} array;

array sentinel = {0, &memory[MEMORY], NULL};

array dummy = {1, &memory[-1], &sentinel};

array * allocated = &dummy;

void check(){
	array * next = allocated;
	while (next != NULL)
	{
		printf("array (%p) : size %2d, segment 0x%x, next %14p \n", next, next->size, next->segment, next->next);
		next = next->next;
	}
	return;
	
}
array * allocate(int size){
	array *nxt = allocated;
	while(nxt->size !=0){
		if(nxt->next->segment - (nxt->segment + nxt->size) >= size){
			array *new = (array*)malloc(sizeof(array));
			new->size=size;
			new->segment = (nxt->segment+nxt->size);
			new->next= nxt->next;
			nxt->next = new;
			return new;
		}
		nxt = nxt->next;

	}
	return NULL;
}

array * create(int size){
	printf("Creating an array of size: %4d...", size);
	array * new = allocate(size);
	if(new != NULL){
		printf("done\n");
	}
	else{
		printf("out of memory\n");
	}
	return new;
}

void delete(array * arr){
	printf("Delete array (%p) of size %d...", arr, arr->size);
	array *nxt = allocated;
	while(nxt->next != arr){
		nxt = nxt->next;
	}
	nxt->next = arr->next;
	free(arr);
	printf("done\n");
}

void set(array * arr, int pos, int val){
	arr->segment[pos] = val;
}

int get(array * arr, int pos){
	return arr->segment[pos];
}

void compact(){
	array * nxt = allocated;
	while(nxt->size != 0){
		int gap = nxt->next->segment - nxt->segment-1;
		if(gap > 0 && nxt->next->size!=0){
			printf("gap: %d\n", gap);
			for(int i =0; i < nxt->next->size; i++){
				nxt->next->segment[i-gap] = nxt->next->segment[i];
			}
			nxt->next->segment -= gap;
		}
		nxt=nxt->next;
	}
}

int main(){
	check();

	array *a = create(0x20);
	check();

	array *b = create(0x30);
	check();

	delete(a);
	check();

	array * c = create(0x10);
	check();

	compact();
	check();

	delete(b);
	delete(c);
	check();

	return 0;
}