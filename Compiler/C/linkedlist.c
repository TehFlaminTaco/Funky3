#include <stdio.h>

#ifndef LINKEDLIST_C
#define LINKEDLIST_C

#include "linkedlist.h"

#include "var.h"

LinkedVarList* LinkedListNew(){
    LinkedVarList* list = tgc_calloc(&gc, 1, sizeof(LinkedVarList));
    list->first = NULL;
    return list;
}

Var* LinkedListGet(LinkedVarList* list, int index){
    Linklett* current = list->first;
    if(current == NULL){
        return NULL;
    }

    for(int i = 0; i < index; i++){
        current = current->next;
        if(current == NULL){
            return NULL;
        }
    }

    return current->var;
};

void LinkedListInsert(LinkedVarList* list, Var* var, int index){
    Linklett* current = list->first;
    if(current == NULL){
        list->first = tgc_calloc(&gc, 1, sizeof(Linklett));
        list->first->var = var;
        list->first->next = NULL;
        return;
    }

    for(int i = 0; i < index; i++){
        current = current->next;
        if(current == NULL){
            current->next = tgc_calloc(&gc, 1, sizeof(Linklett));
            current->next->var = var;
            current->next->next = NULL;
            return;
        }
    }

    Linklett* new = tgc_calloc(&gc, 1, sizeof(Linklett));
    new->var = var;
    new->next = current->next;
    current->next = new;
    return;
}

void LinkedListPush(LinkedVarList* list, Var* var){
    Linklett* new = tgc_calloc(&gc, 1, sizeof(Linklett));
    new->var = var;
    new->next = list->first;
    list->first = new;
    return;
}

Var* LinkedListPop(LinkedVarList* list){
    Linklett* current = list->first;
    if(current == NULL){
        return NULL;
    }
    list->first = current->next;
    Var* var = current->var;
    tgc_free(&gc, current);
    return var;
}

Var* LinkedListRemove(LinkedVarList* list, int index){
    DebugPrint("LinkedListRemove: start\n");
    Linklett* current = list->first;
    if(current == NULL){
        DebugPrint("LinkedListRemove: Empty list\n");
        return &UNDEFINED;
    }

    if(index == 0){
        list->first = current->next;
        Var* var = current->var;
        tgc_free(&gc, current);
        DebugPrint("LinkedListRemove: done\n");
        return var;
    }

    for(int i = 0; i < (index-1); i++){
        DebugPrint("LinkedListRemove: (%i, %p)\n", i, current);
        current = current->next;
        if(current == NULL){
            DebugPrint("LinkedListRemove: index out of range\n");
            return &UNDEFINED;
        }
    }
    if(current->next == NULL){
        DebugPrint("LinkedListRemove: index out of range\n");
        return &UNDEFINED;
    }
    DebugPrint("LinkedListRemove: current = %p\n", current);
    Linklett* toRemove = current->next;
    DebugPrint("LinkedListRemove: toRemove = %p\n", toRemove);
    current->next = toRemove->next;
    DebugPrint("LinkedListRemove: current->next = %p\n", current->next);
    Var* var = toRemove->var;
    DebugPrint("LinkedListRemove: Finshed removing.");
    tgc_free(&gc, toRemove);
    DebugPrint("LinkedListRemove: Finshed freeing.");
    return var;
}

Var* LinkedListRemoveByValue(LinkedVarList* list, Var* var){
    DebugPrint("LinkedListRemoveByValue: start(%p, %p)\n", list, var);
    int i = 0;
    for(Linklett* current = list->first; current != NULL; current = current->next){
        DebugPrint("LinkedListRemoveByValue: current = (%i, %p)\n", i, current);
        if(current->var == var){
            DebugPrint("LinkedListRemoveByValue: found (%i, %p)\n", i, current);
            return LinkedListRemove(list, i);
        }
        i++;
    }
    DebugPrint("LinkedListRemoveByValue: not found\n");
    return &UNDEFINED;
}

void LinkedListFree(LinkedVarList* list){
    Linklett* current = list->first;
    Linklett* toRemove;
    while(current != NULL){
        toRemove = current;
        current = current->next;
        tgc_free(&gc, toRemove);
    }
    tgc_free(&gc, list);
    return;
}

int LinkedListLength(LinkedVarList* list){
    Linklett* current = list->first;
    int length = 0;
    while(current != NULL){
        length++;
        current = current->next;
    }
    return length;
}

#define ForeachInLinkedList(list, i) for(Linklett* i = list->first; i != NULL; i = var->next)

#endif