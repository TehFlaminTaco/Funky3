#ifndef LISTMETA_C
#define LISTMETA_C

#include <stdio.h>

#include "../var.h"
#include "../funky3.h"
#include "../hashmap.h"
#include "../linkedlist.h"

Var* ListToString(Var* scope, Var* args){
    DebugPrint("ListToString\n");
    Var* list = ArgVarGet(args, 0, "obj");
    if(list -> type != VAR_LIST){
        DebugPrint("ListToString: not a list\n");
        return &UNDEFINED;
    }
    DebugPrint("ListToString: %p\n", list);
    HashMap* map = (HashMap*)list -> value;
    LinkedVarList* strings = LinkedListNew();
    LinkedListPush(strings, VarNewConstString("["));

    // Iterate through all integer elements in the map.
    int highestInt = 0;
    int totalStringLength = 1;
    Var* current = HashMapGet(map, VarNewNumber(0));
    while(!ISUNDEFINED(current)){
        DebugPrint("ListToString: %i, %p\n", highestInt, current);
        Var* curString = VarAsCode(current);
        DebugPrint("ListToString: %s\n", (char*)(intptr_t)(curString -> value));
        LinkedListPush(strings, curString);
        totalStringLength += strlen((char*) curString -> value);
        current = HashMapGet(map, VarNewNumber(++highestInt));
        if(!ISUNDEFINED(current)){
            LinkedListPush(strings, VarNewConstString(", "));
            totalStringLength += 2;
        }
    }
    highestInt--;

    LinkedListPush(strings, VarNewConstString("]"));
    totalStringLength++;
    DebugPrint("ListToString: Total Length %i\n", totalStringLength);
    // Create a new string with the concatenated strings.
    char* string = tgc_calloc(&gc, totalStringLength+1, sizeof(char));
    int index = totalStringLength;
    string[totalStringLength] = '\0';
    for(Linklett* current = strings->first; current != NULL; current = current->next){
        char* curString = (char*) current->var->value;
        int curStringLength = strlen(curString);
        index -= curStringLength;
        strncpy(string + index, curString, curStringLength);
    }
    LinkedListFree(strings);
    Var* result = VarNewString(string);
    tgc_free(&gc, string);
    return result;
    /*for(int j = 0; j < map -> capacity; j++){

    }*/
}

Var* _listIter(Var* scope, Var* args){
    DebugPrint("_listIter\n");
    Var* keys = VarRawGet(scope, VarNewConstString("keys"));
    Var* index = VarRawGet(scope, VarNewConstString("index"));
    if(keys -> type != VAR_LIST){
        DebugPrint("_listIter: not a list\n");
        return &UNDEFINED;
    }
    if(index -> type != VAR_NUMBER){
        DebugPrint("_listIter: index is not a number\n");
        return &UNDEFINED;
    }
    double j;
    memcpy(&j, &index -> value, sizeof(double));
    int i = (int)j;
    Var* val = VarRawGet(keys, VarNewNumber(i));
    if(ISUNDEFINED(val)){
        DebugPrint("_listIter: end\n");
        return &UNDEFINED;
    }
    VarRawSet(scope, VarNewConstString("index"), VarNewNumber(i + 1));
    DebugPrint("_listIter: Sent\n");
    return val;
}

Var* ListIterator(Var* scope, Var* args){
    DebugPrint("ListIterator\n");
    Var* list = ArgVarGet(args, 0, "obj");
    if(list -> type != VAR_LIST){
        DebugPrint("ListIterator: not a list\n");
        return &UNDEFINED;
    }
    DebugPrint("ListIterator: %p\n", list);
    Var* keysList = VarNewList();

    HashMap* map = (HashMap*)list -> value;
    int j = 0;
    for(int i = 0; i < map -> capacity; i++){
        KVLinklett* current = map->values[i]->first;
        while(current != NULL){
            VarRawSet(keysList, VarNewNumber(j++), current->key);
            current = current->next;
        }
    }

    Var* func = VarNewFunction(_listIter);
    VarFunction* funcObj = (VarFunction*)func -> value;
    funcObj -> scope = VarNewList();
    VarRawSet(funcObj -> scope, VarNewConstString("keys"), keysList);
    VarRawSet(funcObj -> scope, VarNewConstString("index"), VarNewNumber(0));
    return func;
}


Var* ListLen(Var* scope, Var* args){
    Var* list = ArgVarGet(args, 0, "obj");
    if(list -> type != VAR_LIST){
        return &UNDEFINED;
    }
    int len = 0;
    while(!ISUNDEFINED(VarRawGet(list, VarNewNumber(len)))){
        len++;
    }
    return VarNewNumber(len);
}

void PopulateListMeta(Var* metatable){
    VarRawSet(metatable, VarNewConstString("tostring"), VarNewFunction(ListToString));
    VarRawSet(metatable, VarNewConstString("tocode"),   VarNewFunction(ListToString));

    VarRawSet(metatable, VarNewConstString("iter"), VarNewFunction(ListIterator));
    VarRawSet(metatable, VarNewConstString("len"), VarNewFunction(ListLen));
}

#endif