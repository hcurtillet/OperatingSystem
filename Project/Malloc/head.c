#include "head.h"
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

#define HEAD (sizeof(struct head))

#define MIN(size) (((size>(8))?(size):(8)))

#define LIMIT(size) (MIN(0) + HEAD + size)

#define MAGIC(memory) ((struct head *)memory-1)

#define HIDE(block) (void*)((struct head *) block +1)

#define ALIGN 8

#define ARENA (64*1024)

struct head * after(struct head *block){
    return (struct head *)((void *)block + block->size + HEAD);
}

struct head * before(struct head *block){
    return (struct head*)((void*)block - block->bsize - HEAD);
}

struct head * split(struct head * block, int size){
    int rsize = block->size - size-HEAD;
    block->size = size;

    struct head * splt = malloc(sizeof(struct head*));
    splt->bfree = block->free;
    splt->bsize = block->size;
    splt->size = rsize;
    splt->free = TRUE;
    splt->next = block->next;
    splt->prev = block;

    struct head * aft = block->next;
    aft->prev = splt;
    aft->bsize = splt->size;
    aft->bfree = splt->free;

    block->next = splt;

    return splt;

}