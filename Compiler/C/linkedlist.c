#include <stdio.h>

#ifndef LINKEDLIST_C
#define LINKEDLIST_C

#include "linkedlist.h"

#include "var.h"

LinkedVarList* LinkedListNew(){
    LinkedVarList* list = calloc(1, sizeof(LinkedVarList));
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
        list->first = calloc(1, sizeof(Linklett));
        list->first->var = var;
        list->first->next = NULL;
        return;
    }

    for(int i = 0; i < index; i++){
        current = current->next;
        if(current == NULL){
            current->next = calloc(1, sizeof(Linklett));
            current->next->var = var;
            current->next->next = NULL;
            return;
        }
    }

    Linklett* new = calloc(1, sizeof(Linklett));
    new->var = var;
    new->next = current->next;
    current->next = new;
    return;
}

void LinkedListPush(LinkedVarList* list, Var* var){
    Linklett* new = calloc(1, sizeof(Linklett));
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
    free(current);
    return var;
}

Var* LinkedListRemove(LinkedVarList* list, int index){
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

    Linklett* toRemove = current->next;
    current->next = toRemove->next;
    Var* var = toRemove->var;
    free(toRemove);
    return var;
}

Var* LinkedListRemoveByValue(LinkedVarList* list, Var* var){
    int i = 0;
    for(Linklett* current = list->first; current != NULL; current = current->next){
        if(current->var == var){
            return LinkedListRemove(list, i);
        }
        i++;
    }
    return &NIL;
}

void LinkedListFree(LinkedVarList* list){
    Linklett* current = list->first;
    Linklett* toRemove;
    while(current != NULL){
        toRemove = current;
        current = current->next;
        free(toRemove);
    }
    free(list);
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