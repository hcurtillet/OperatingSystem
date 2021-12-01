#include "dlmalllist.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    extern struct head* arena;
    new();
    flistprint();
    struct head * a = dalloc(12);
    //flistprint();
    struct head * b = dalloc(28);
    // flistprint();
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