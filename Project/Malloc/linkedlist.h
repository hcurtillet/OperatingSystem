#include <stdio.h>
#include <stdlib.h>


typedef struct _link {
  int size; //Block size
  struct _link *next;
  struct _link *previous;
} link_t, *list_t;