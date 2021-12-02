#include "dlmall.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char **argv){
    extern struct head* arena;
    extern struct head * flist;
    struct allocatedlist * alist = NULL;
    new();
    printf("The arena is at (%p) and the size is %d\n",arena, arena->size);
    flist = arena;
    int n = atoi(argv[1]);
    int min = atoi(argv[2]);
    int max = atoi(argv[3]);
    srand(time(NULL));
    int maxalloc = atoi(argv[4]);
    printf("We'll do %d allocation(s)\nThe min size is %d and the max size is %d\nWe allow a maximum of %d blocks allocated\n", n, min, max, maxalloc);
    int currentnballoc = 0;
    int nballoc=0;
    struct head * allocated[maxalloc];
    for(int i=0; i<maxalloc; i++){
        allocated[i] = NULL;
    }
    printf("Start\n");
    while(nballoc<n){
        int r = rand()%maxalloc;
        printf("%d\n",r);
        if(allocated[r]==NULL){
            int size = (rand()%(max-min))+min;
            printf("No allocation: We allocate a new block of %d at %d\n", size, r);
            allocated[r] = dalloc(size);
            nballoc++;
        }
        else{
            printf("We'll free a block %d of size %d\n",r, allocated[r]->size);
            dfree(allocated[r]);
            allocated[r] = NULL;
        }
    }
    for(int i=0; i<maxalloc; i++){
        if(allocated[i]!=NULL){
            dfree(allocated[i]);
        }
    }
    return 0;
}