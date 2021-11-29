#include "dlmall.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
typedef struct allocatedlist{
    struct head * val;
    struct allocatedlist * next;
};
void alistprint(struct allocatedlist* list){
    printf("Allocated list:");
    struct allocatedlist * current = list;
    while(current != NULL){
        printf("(%p)--", current->val);
        current = current->next;
    }
    printf("\n");
}
struct allocatedlist * add(struct head * val, struct allocatedlist * list){
    struct allocatedlist * new = malloc(sizeof(struct allocatedlist *));
    new->val = val;
    new->next = list;
    printf("%p\n", val);
    return new;
}



struct head * getrandomheap(int n, struct allocatedlist* list){
    alistprint(list);
    struct heap * result = NULL;
    if(n == 1){
        struct head * remove = list->val;
        printf("We free the block (%p)", remove);printf("with the size %d\n", remove->size);
        dfree(remove);
        return NULL;
    }
    // srand(time(NULL));
    struct allocatedlist * current = list;
    struct allocatedlist * next = current->next;
    int r = rand()%(n-1);
    printf("rand = %d\n", r);
    for(int i=0; i<r; i++){
        current = current->next;
        next = next->next;
    }
    current->next = next->next;
    struct head * remove = next->val;
    printf("We free the block (%p)", remove);printf("with the size %d\n", remove->size);
    dfree(remove);
    return list;
}



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
    for(int i=0; i<n; i++){
        printf("i: %d\n",i);
        double r = ((double) rand() / (RAND_MAX));
        if(currentnballoc == 0){
            int size = (rand() % (max-min))+min;
            printf("No allocation: We allocate a new block of %d\n", size);
            struct head * new = dalloc(size);
            alist = add(new, alist);
            currentnballoc++;
            alistprint(alist);
        }
        else if(r<0.5 && currentnballoc<maxalloc){
            int size = (rand() % (max-min))+min;
            printf("We allocate a new block of %d\n", size);
            struct head * new = dalloc(size);
            alist = add(new, alist);
            currentnballoc++;
            alistprint(alist);
        }
        else{
            printf("We'll free a block\n");
            alist = getrandomheap(currentnballoc, alist);
            currentnballoc--;
        }
    }
    for(int i=0; i<currentnballoc; i++){
        alist = getrandomheap(currentnballoc, alist);
        currentnballoc--;
    }
    return 0;
}