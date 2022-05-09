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

LinkedKVList* LinkedKVListNew();
Var* LinkedKVListGet(LinkedKVList* list, Var* key);
Var* LinkedKVListGetKey(LinkedKVList* list, Var* key);
void LinkedKVListInsert(LinkedKVList* list, Var* key, Var* var);
Var* LinkedKVListRemove(LinkedKVList* list, Var* key);
void LinkedKVListFree(LinkedKVList* list);
int LinkedKVListLength(LinkedKVList* list);
int LinkedKVListHasValue(LinkedKVList* list, Var* var);

#endif