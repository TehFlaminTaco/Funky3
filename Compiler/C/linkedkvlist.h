#ifndef LINKEDKVLIST_H
#define LINKEDKVLIST_H

typedef struct _KVLinklett {
    struct _VAR* key;
    struct _VAR* var;
    struct _KVLinklett* next;
} KVLinklett;

typedef struct _LinkedKVList {
    KVLinklett* first;
} LinkedKVList;

#endif