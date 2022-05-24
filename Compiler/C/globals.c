#ifndef GLOBALS_C
#define GLOBALS_C

#include <stdio.h>

#include "funky3.h"
#include "hashmap.h"

#include "libs/list.c"
#include "libs/string.c"
#include "libs/math.c"

Var* Print(Var* scope, Var* args){
    DebugPrint("PRINT\n");
    int index = 0;
    Var* arg = ArgVarGet(args, index, NULL);
    if(ISUNDEFINED(arg)){
        DebugPrint("PRINT: No arguments\n");
    }
    while(!ISUNDEFINED(arg)){
        DebugPrint("%i, %i\n", index, arg -> type);
        printf("%s", VarAsString(arg)->value);
        arg = ArgVarGet(args, ++index, NULL);
        if(arg != NULL)
            printf("\t");
    }
    printf("\n");
    return &NIL;
}

Var* _values(Var* scope, Var* args){
    DebugPrint("_values\n");
    Var* values = VarRawGet(scope, VarNewString("values"));
    Var* index = VarRawGet(scope, VarNewString("index"));
    if(values -> type != VAR_LIST){
        DebugPrint("_values: not a list\n");
        return &UNDEFINED;
    }
    if(index -> type != VAR_NUMBER){
        DebugPrint("_values: index is not a number\n");
        return &UNDEFINED;
    }
    double j;
    memcpy(&j, &index -> value, sizeof(double));
    int i = (int)j;
    Var* val = VarRawGet(values, VarNewNumber(i));
    if(ISUNDEFINED(val)){
        DebugPrint("_values: end\n");
        return &UNDEFINED;
    }
    VarRawSet(scope, VarNewString("index"), VarNewNumber(i + 1));
    DebugPrint("_values: Sent\n");
    return val;
}

Var* Values(Var* scope, Var* args){
    DebugPrint("Values\n");
    Var* list = ArgVarGet(args, 0, "obj");
    if(list -> type != VAR_LIST){
        DebugPrint("Values: not a list\n");
        return &UNDEFINED;
    }
    DebugPrint("Values: %p\n", list);
    Var* valuesList = VarNewList();

    HashMap* map = (HashMap*)list -> value;
    int j = 0;
    for(int i = 0; i < map -> capacity; i++){
        KVLinklett* current = map->values[i]->first;
        while(current != NULL){
            VarRawSet(valuesList, VarNewNumber(j++), current->var);
            current = current->next;
        }
    }

    Var* func = VarNewFunction(_values);
    VarFunction* funcObj = (VarFunction*)func -> value;
    funcObj -> scope = VarNewList();
    VarRawSet(funcObj -> scope, VarNewString("values"), valuesList);
    VarRawSet(funcObj -> scope, VarNewString("index"), VarNewNumber(0));
    return func;
}

Var* _pairs(Var* scope, Var* args){
    DebugPrint("_pairs\n");
    Var* pairs = VarRawGet(scope, VarNewString("pairs"));
    Var* index = VarRawGet(scope, VarNewString("index"));
    if(pairs -> type != VAR_LIST){
        DebugPrint("_pairs: not a list\n");
        return &UNDEFINED;
    }
    if(index -> type != VAR_NUMBER){
        DebugPrint("_pairs: index is not a number\n");
        return &UNDEFINED;
    }
    double j;
    memcpy(&j, &index -> value, sizeof(double));
    int i = (int)j;
    Var* val = VarRawGet(pairs, VarNewNumber(i));
    if(ISUNDEFINED(val)){
        DebugPrint("_pairs: end\n");
        return &UNDEFINED;
    }
    VarRawSet(scope, VarNewString("index"), VarNewNumber(i + 1));
    DebugPrint("_pairs: Sent\n");
    return val;
}

Var* Pairs(Var* scope, Var* args){
    DebugPrint("Pairs\n");
    Var* list = ArgVarGet(args, 0, "obj");
    if(list -> type != VAR_LIST){
        DebugPrint("Pairs: not a list\n");
        return &UNDEFINED;
    }
    DebugPrint("Pairs: %p\n", list);
    Var* pairsList = VarNewList();

    HashMap* map = (HashMap*)list -> value;
    int j = 0;
    for(int i = 0; i < map -> capacity; i++){
        KVLinklett* current = map->values[i]->first;
        while(current != NULL){
            Var* pair = VarNewList();
            // Honestly ArgVarSet just does everything I want from this.
            ArgVarSet(pair, 0, "key", current->key);
            ArgVarSet(pair, 1, "value", current->var);
            VarRawSet(pairsList, VarNewNumber(j++), pair);
            current = current->next;
        }
    }

    Var* func = VarNewFunction(_pairs);
    VarFunction* funcObj = (VarFunction*)func -> value;
    funcObj -> scope = VarNewList();
    VarRawSet(funcObj -> scope, VarNewString("pairs"), pairsList);
    VarRawSet(funcObj -> scope, VarNewString("index"), VarNewNumber(0));
    return func;
}

void PopulateGlobals(Var* globals){
    // Populate the globals with some basic functions.
    VarRawSet(globals, VarNewString("pairs"), VarNewFunction(Pairs));
    VarRawSet(globals, VarNewString("print"), VarNewFunction(Print));
    VarRawSet(globals, VarNewString("values"), VarNewFunction(Values));

    // Libs

    // List
    Var* list = VarNewList();
    VarRawSet(globals, VarNewString("list"), list);
    PopulateListLib(list);

    // String
    Var* string = VarNewList();
    VarRawSet(globals, VarNewString("string"), string);
    PopulateStringLib(string);

    // Math
    Var* math = VarNewList();
    VarRawSet(globals, VarNewString("math"), math);
    PopulateMathLib(math);
}

#endif