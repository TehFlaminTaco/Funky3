#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct _Linklett {
    struct _VAR* var;
    struct _Linklett* next;
} Linklett;

typedef struct _LinkedVarList {
    Linklett* first;
} LinkedVarList;

#endif