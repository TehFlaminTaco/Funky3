#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct _Linklett {
    struct _VAR* var;
    struct _Linklett* next;
} Linklett;

typedef struct _LinkedVarList {
    Linklett* first;
} LinkedVarList;


LinkedVarList* LinkedListNew();
Var* LinkedListGet(LinkedVarList* list, int index);
void LinkedListInsert(LinkedVarList* list, Var* var, int index);
void LinkedListPush(LinkedVarList* list, Var* var);
Var* LinkedListPop(LinkedVarList* list);
Var* LinkedListRemove(LinkedVarList* list, int index);
Var* LinkedListRemoveByValue(LinkedVarList* list, Var* var);
void LinkedListFree(LinkedVarList* list);
int LinkedListLength(LinkedVarList* list);

#endif