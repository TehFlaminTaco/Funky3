#ifndef VARLIST_H
#define VARLIST_H

#include "var.h"
#include "linkedkvlist.c"

typedef struct _HashMap {
    LinkedKVList** values;
    int capacity;
} HashMap;

#endif