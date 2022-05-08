#include <stdio.h>

#ifndef LINKEDKVLIST_C
#define LINKEDKVLIST_C

#include "linkedkvlist.h"

#include "var.h"

LinkedKVList* LinkedKVListNew(){
    LinkedKVList* list = calloc(1, sizeof(LinkedKVList));
    list->first = NULL;
    return list;
}

Var* LinkedKVListGet(LinkedKVList* list, Var* key){
    KVLinklett* current = list->first;
    if(current == NULL){
        return &UNDEFINED;
    }
    for(; current != NULL; current = current->next){
        if(VarEquals(current->key, key)){
            return current->var;
        }
    }
    return &UNDEFINED;
}

Var* LinkedKVListGetKey(LinkedKVList* list, Var* key){
    KVLinklett* current = list->first;
    if(current == NULL){
        return &UNDEFINED;
    }
    for(; current != NULL; current = current->next){
        if(VarEquals(current->key, key)){
            return current->key;
        }
    }
    return &UNDEFINED;
}

void LinkedKVListInsert(LinkedKVList* list, Var* key, Var* var){
    DebugPrint("LinkedKVListInsert\n");
    KVLinklett* current = list->first;
    if(current == NULL){
        DebugPrint("LinkedKVListInsert: first is null\n");
        list->first = calloc(1, sizeof(KVLinklett));
        list->first->var = var;
        list->first->key = key;
        list->first->next = NULL;
        return;
    }

    KVLinklett* last = current;
    for(; current != NULL; current = current->next){
        // If our key is equal, replace it.
        if(VarEquals(current->key, key)){
            DebugPrint("LinkedKVListInsert: found key\n");
            current->var = var;
            return;
        }
        last = current;
    }

    DebugPrint("LinkedKVListInsert: reached end of list\n");
    KVLinklett* new = calloc(1, sizeof(KVLinklett));
    new->var = var;
    new->key = key;
    new->next = NULL;
    last->next = new;
    DebugPrint("LinkedKVListInsert: done\n");
    return;
}

Var* LinkedKVListRemove(LinkedKVList* list, Var* key){
    KVLinklett* current = list->first;
    if(current == NULL){
        return NULL;
    }
    for(; current->next != NULL; current = current->next){
        if(VarEquals(current->next->key, key)){
            KVLinklett* toRemove = current->next;
            current->next = toRemove->next;
            Var* var = toRemove->var;
            free(toRemove);
            return var;
        }
    }
    return NULL;
}

void LinkedKVListFree(LinkedKVList* list){
    KVLinklett* current = list->first;
    KVLinklett* toRemove;
    while(current != NULL){
        toRemove = current;
        current = current->next;
        free(toRemove);
    }
    free(list);
    return;
}

int LinkedKVListLength(LinkedKVList* list){
    KVLinklett* current = list->first;
    int length = 0;
    while(current != NULL){
        length++;
        current = current->next;
    }
    return length;
}

#define ForeachInLinkedKVList(list, i) for(KVLinklett* i = list->first; i != NULL; i = var->next)

#endif