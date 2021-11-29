#include <stdlib.h>
#include <inttypes.h>

struct head
{
    uint16_t bfree; //Status of the block before
    uint16_t bsize; //Size of the block before
    uint16_t free; //Status of this block
    uint16_t size; //Size of the block
    struct head *next;
    struct head *prev;
};

struct head * after(struct head* block);

struct head* beofre(struct head* block);

struct head * split(struct head * block, int size);