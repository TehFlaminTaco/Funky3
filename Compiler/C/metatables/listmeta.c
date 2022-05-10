#ifndef LISTMETA_C
#define LISTMETA_C

#include <stdio.h>

#include "../var.h"
#include "../funky3.h"
#include "../hashmap.h"

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
    LinkedListPush(strings, VarNewString("["));

    // Iterate through all integer elements in the map.
    int highestInt = 0;
    int totalStringLength = 1;
    Var* current = HashMapGet(map, VarNewNumber(0));
    while(!ISUNDEFINED(current)){
        DebugPrint("ListToString: %i, %p\n", highestInt, current);
        Var* curString = VarAsCode(current);
        DebugPrint("ListToString: %s\n", (char*)(intptr_t)(curString -> value));
        LinkedListPush(strings, curString);
        totalStringLength += strlen(curString -> value);
        current = HashMapGet(map, VarNewNumber(++highestInt));
        if(!ISUNDEFINED(current)){
            LinkedListPush(strings, VarNewString(", "));
            totalStringLength += 2;
        }
    }
    highestInt--;

    LinkedListPush(strings, VarNewString("]"));
    totalStringLength++;
    DebugPrint("ListToString: Total Length %i\n", totalStringLength);
    // Create a new string with the concatenated strings.
    char* string = calloc(totalStringLength+1, sizeof(char));
    int index = totalStringLength;
    string[totalStringLength] = '\0';
    for(Linklett* current = strings->first; current != NULL; current = current->next){
        char* curString = current->var->value;
        int curStringLength = strlen(curString);
        index -= curStringLength;
        strncpy(string + index, curString, curStringLength);
    }
    LinkedListFree(strings);
    Var* result = VarNewString(string);
    free(string);
    return result;
    /*for(int j = 0; j < map -> capacity; j++){

    }*/
}

void PopulateListMeta(Var* metatable){
    VarRawSet(metatable, VarNewString("tostring"), VarNewFunction(ListToString));
    VarRawSet(metatable, VarNewString("tocode"),   VarNewFunction(ListToString));
}

#endif