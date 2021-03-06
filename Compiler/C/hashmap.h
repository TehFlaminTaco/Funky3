#ifndef VARLIST_H
#define VARLIST_H

#include "var.h"
#include "linkedkvlist.h"

typedef struct _HashMap {
    int capacity;
    Var* parent;
    Var* withValue;
    LinkedKVList** values;
} HashMap;

HashMap* HashMapNew(int capacity);

void HashMapSet(HashMap* map, Var* key, Var* value);
Var* HashMapGet(HashMap* map, Var* key);
Var* HashMapGetKey(HashMap* map, Var* key);
Var* HashMapRemove(HashMap* map, Var* key);
int HashMapHasValue(HashMap* map, Var* value);

#endif