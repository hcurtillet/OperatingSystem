#include "dlmall.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    extern struct head* arena;
    extern struct head * flist;
    new();
    printf("The arena is at (%p) and the size is %d\n",arena, arena->size);
    flist = arena;
    flistprint();
    struct head * a = dalloc(2048);
    flistprint();
    struct head * b = dalloc(4096);
    flistprint();
    struct head * c = dalloc(1024);
    flistprint();
    dfree(a);
    flistprint();
    dfree(b);
    flistprint();
    dfree(c);
    flistprint();
    return 0;
}