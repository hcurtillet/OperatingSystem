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

struct head* before(struct head* block);

struct head * split(struct head * block, int size);

struct head * new();

void detach(struct head * block);

void insert(struct head *block);

int adjust(int size);

struct head * find(int size);

void *dalloc(size_t request);

void dfree(void *memory);

void flistprint();

int sanity();

struct head * merge(struct head * block);

int findlist(size_t size);