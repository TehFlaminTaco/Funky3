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

Var* LinkedKVListGet(LinkedKVList* list, int index){
    KVLinklett* current = list->first;
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

Var* LinkedKVListGetKey(LinkedKVList* list, Var* key){
    KVLinklett* current = list->first;
    if(current == NULL){
        return NULL;
    }
    for(; current != NULL; current = current->next){
        if(VarEquals(current->key, key) == 0){
            return current->var;
        }
    }
    return NULL;
}

void LinkedKVListInsert(LinkedKVList* list, Var* var, int index, Var* key){
    KVLinklett* current = list->first;
    if(current == NULL){
        list->first = calloc(1, sizeof(KVLinklett));
        list->first->var = var;
        list->first->key = key;
        list->first->next = NULL;
        return;
    }

    for(int i = 0; i < index; i++){
        current = current->next;
        if(current == NULL){
            current->next = calloc(1, sizeof(KVLinklett));
            current->next->var = var;
            current->next->key = key;
            current->next->next = NULL;
            return;
        }
    }

    KVLinklett* new = calloc(1, sizeof(KVLinklett));
    new->var = var;
    new->key = key;
    new->next = current->next;
    current->next = new;
    return;
}

void LinkedKVListPush(LinkedKVList* list, Var* var, Var* key){
    KVLinklett* new = calloc(1, sizeof(KVLinklett));
    new->var = var;
    new->key = VarNew(key);
    new->next = list->first;
    list->first = new;
    return;
}

Var* LinkedKVListPop(LinkedKVList* list){
    KVLinklett* current = list->first;
    if(current == NULL){
        return NULL;
    }
    list->first = current->next;
    Var* var = current->var;
    free(current);
    return var;
}

Var* LinkedKVListRemove(LinkedKVList* list, int index){
    KVLinklett* current = list->first;
    if(current == NULL){
        return NULL;
    }

    for(int i = 0; i < index; i++){
        current = current->next;
        if(current == NULL){
            return NULL;
        }
    }

    KVLinklett* toRemove = current->next;
    current->next = toRemove->next;
    Var* var = toRemove->var;
    free(toRemove);
    return var;
}

Var* LinkedKVListKeyRemove(LinkedKVList* list, Var* key){
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