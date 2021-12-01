#include "dlmalllist.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0

#define HEAD (sizeof(struct head))

#define MIN(size) (((size>(8))?(size):(8)))

#define LIMIT(size) (MIN(0) + HEAD + size)

#define MAGIC(memory) ((struct head *)memory-1)

#define HIDE(block) (void*)((struct head *) block +1)

#define ALIGN 8

#define ARENA (64*1024)

#define NBLIST 4

struct head * arena = NULL;

struct head * flist[NBLIST];

int currentlist;


struct head * after(struct head *block){
    return (struct head *)((char *)block + block->size + HEAD);
}

struct head * before(struct head *block){
    return (struct head*)((char*)block - block->bsize - HEAD);
}

struct head * split(struct head * block, int size){
    int rsize = block->size - size-HEAD;
    block->size = size;
    block->free=FALSE;
    printf("We split\n");
    struct head * splt = after(block);
    splt->bfree = block->free;
    splt->bsize = block->size;
    splt->size = rsize;
    splt->free = TRUE;
    splt->next = block->next;
    splt->prev = block;

    struct head * aft = block->next;
    if(aft !=NULL){
        aft->prev = splt;
        aft->bsize = splt->size;
        aft->bfree = splt->free;
    }

    block->next = splt;
    return splt;

}

struct head *new(){
    if(arena!=NULL){
        printf("One arena in already allocated\n");
        return NULL;
    }
    //using mmap
    struct head * new = mmap(
        NULL, 
        ARENA,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0
    );
    if(new == MAP_FAILED){
        printf("mmap failed: error %d\n", errno);
        return NULL;
    }
    //Make room for the head and dummy

    unsigned int size = ARENA -2*HEAD;
    new->bfree = FALSE;
    new->bsize = 0;
    new->free = TRUE;
    new->size = size;

    struct head * sentinel = after(new);
    sentinel->bfree = new->free;
    sentinel->bsize = new->size;
    sentinel->size = 0;
    sentinel->free = FALSE;
    sentinel->prev = new;

    new->next=sentinel;
    arena = (struct head*)new;
    printf("The arena is at (%p) and the size is %d\n",arena, ARENA);
    struct head * tmp = arena;
    for(int i=0; i<NBLIST; i++){
        tmp->size = ARENA/NBLIST;
        tmp->free = TRUE;
        tmp->bfree = FALSE;
        tmp->bsize = 0;
        tmp->prev = NULL;
        tmp->next = NULL;
        flist[i] = tmp;
        tmp = after(tmp);
    }
    return new;
}

void detach(struct head * block){
    block->free= FALSE;
    if(block->next != NULL){
        struct head *nxt = block->next;
        nxt->prev = block->prev;
    }
    if(block->prev != NULL){
        block->prev->next = block->next;
    }
    else{
        flist[currentlist] = block->next;
    }
}

void insert(struct head *block){
    block->free = TRUE;
    block->next = flist[currentlist];
    block->prev = NULL;
    block->bfree = FALSE;
    if(flist[currentlist] != NULL){
        flist[currentlist]->prev = block;
    }
    flist[currentlist] = block;

}

int adjust(int size){
    int factor = size/ALIGN;
    if(size%ALIGN == 0){
        return factor*ALIGN + HEAD;
    }
    else{
        return (factor+1)*ALIGN +HEAD;
    }
}

struct head * find(int size){
    currentlist = findlist(size);
    struct head * current = flist[currentlist];
    int sizeajust = adjust(size);
    printf("Real alloc size:%d in the list %d\n",sizeajust,currentlist);
    while(current!=NULL){
        printf("current (%p):%d\n", current, current->size);
        if(current->size == sizeajust){
            detach(current);
            return current;
        }
        if(current->size > sizeajust){
            //printf("Here\n");
            split(current, sizeajust-HEAD);
            //flistprint();
            detach(current);
            //printf("Then\n");
            //flistprint();
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void *dalloc(size_t request){
    printf("dalloc memory for %d\n", request);
    if(request <= 0){
        return NULL;
    }
    struct head *taken= find(request);
    if(taken == NULL){
        return NULL;
    }
    else{
        return taken;
    }
}

void dfree(void *memory){
    if(memory !=NULL){
        struct head *block = memory;
        printf("Free block (%p), size:%d, bsize:%d, bfree%d\n",block, block->size, block->bsize, block->bfree);    
        currentlist = findlist(block->size);
        struct head *aft = after(block);
        block->free = TRUE;
        aft->bfree = block->free;
        block = merge(block);
        printf("Size: %d\n", block->size);
        insert(block);
    }
    return ;
}

void flistprint(){
    printf("Start of flistprint-------------------------------------------------------\n");
    for(int i = 0; i<NBLIST; i++){
        if(i<NBLIST-1){
            printf("-------------------------------------List for block with size %d\n", (i+2)*ALIGN);
        }
        else{
            printf("----------------------------------List for other block\n");
        }
        struct head * current = flist[i];
        int i=0;
        while(current!=NULL){
            printf("Heap number %d\n",i);
            printf("The head is at (%p), the size is %d, the previous head is (%p) and the next head is (%p)\n", current, current->size, current->prev, current->next);
            printf("bfree: %d and bsize:%d\n", current->bfree, current->bsize);
            current = current->next;
            i++;
        }
    }
    printf("End of flistprint---------------------------------------------------------\n");
}

int sanity(){
    struct head * current = flist[0];
    while(current != NULL){
        if(current->next != NULL){
            if(current!= current->next->prev){
                return 0;
            }
        }
        if(current->free!=TRUE){
            return 0;
        }
        current = current->next;
    }
    return 1;
}

struct head * merge(struct head * block){
    struct head * aft = after(block);
    if(block->bfree){
        printf("Here for block (%p):%d\n",block, block->bsize);
        struct head *bfr = before(block);
        printf("Before: (%p)\n",bfr);
        detach(bfr);
        bfr->size = bfr->size + block->size+HEAD;
        aft->bsize = bfr->size;
        aft->bfree = block->free;

        block = bfr;
    }
    if(aft->free){
        detach(aft);
        block->size = block->size + aft->size+HEAD;
    }

    return block;
}

int findlist(size_t size){
    int res = (size-1)/ALIGN;
    if(res!=0){
        res--;
    }
    if(res > NBLIST-1){
        res = NBLIST-1;
    }
    return res;
}