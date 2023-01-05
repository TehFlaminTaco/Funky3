#ifndef GLOBALS_C
#define GLOBALS_C

#include <stdio.h>

#include "funky3.h"
#include "hashmap.h"

#define CONSTANT(name, value) VarRawSet(LIBNAME, VarNewConstString(#name), value)
#define ALIAS(name, alias) VarRawSet(LIBNAME, VarNewConstString(#alias), VarRawGet(LIBNAME, VarNewConstString(#name)))

#define LIBNAME list
#include "libs/list.c"
#define LIBNAME string
#include "libs/string.c"
#define LIBNAME math
#include "libs/math.c"
#define LIBNAME draw
#include "libs/draw.c"
#define LIBNAME os
#include "libs/os.c"
#define LIBNAME meta
#include "libs/meta.c"

Var* GlobalPrint(Var* scope, Var* args){
    DebugPrint("PRINT\n");
    int index = 0;
    Var* sep = VarRawGet(args, VarNewConstString("sep"));
    Var* end = VarRawGet(args, VarNewConstString("end"));
    Var* arg = ArgVarGet(args, index, NULL);
    if(ISUNDEFINED(arg)){
        DebugPrint("PRINT: No arguments\n");
    }
    if(ISUNDEFINED(sep)){
        sep = VarNewConstString("\t");
    }else{
        sep = VarAsString(sep);
        if(sep -> type != VAR_STRING){
            sep = VarNewConstString("\t");
        }
    }
    if(ISUNDEFINED(end)){
        end = VarNewConstString("\n");
    }else{
        end = VarAsString(end);
        if(end -> type != VAR_STRING){
            end = VarNewConstString("\n");
        }
    }
    while(!ISUNDEFINED(arg)){
        DebugPrint("%i, %i\n", index, arg -> type);
        printf("%s", (char*) VarAsString(arg)->value);
        arg = ArgVarGet(args, ++index, NULL);
        if(!ISUNDEFINED(arg))
            printf("%s", (char*) sep -> value);
    }
    printf((char*) end -> value);
    return &NIL;
}

// Literally just PRINT with different defaults.
// Bad, I know.
Var* GlobalWrite(Var* scope, Var* args){
    DebugPrint("WRITE\n");
    int index = 0;
    Var* sep = VarRawGet(args, VarNewConstString("sep"));
    Var* end = VarRawGet(args, VarNewConstString("end"));
    Var* arg = ArgVarGet(args, index, NULL);
    if(ISUNDEFINED(arg)){
        DebugPrint("WRITE: No arguments\n");
    }
    if(ISUNDEFINED(sep)){
        sep = VarNewConstString(" ");
    }else{
        sep = VarAsString(sep);
        if(sep -> type != VAR_STRING){
            sep = VarNewConstString(" ");
        }
    }
    if(ISUNDEFINED(end)){
        end = VarNewConstString("");
    }else{
        end = VarAsString(end);
        if(end -> type != VAR_STRING){
            end = VarNewConstString("");
        }
    }
    while(!ISUNDEFINED(arg)){
        DebugPrint("%i, %i\n", index, arg -> type);
        printf("%s", (char*) VarAsString(arg)->value);
        arg = ArgVarGet(args, ++index, NULL);
        if(!ISUNDEFINED(arg))
            printf("%s", (char*) sep -> value);
    }
    printf((char*) end -> value);
    return &NIL;
}

Var* _values(Var* scope, Var* args){
    DebugPrint("_values\n");
    Var* values = VarRawGet(scope, VarNewConstString("values"));
    Var* index = VarRawGet(scope, VarNewConstString("index"));
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
    VarRawSet(scope, VarNewConstString("index"), VarNewNumber(i + 1));
    DebugPrint("_values: Sent\n");
    return val;
}

Var* GlobalValues(Var* scope, Var* args){
    DebugPrint("GlobalValues\n");
    Var* list = ArgVarGet(args, 0, "obj");
    if(list -> type != VAR_LIST){
        DebugPrint("GlobalValues: not a list\n");
        return &UNDEFINED;
    }
    DebugPrint("GlobalValues: %p\n", list);
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
    VarRawSet(funcObj -> scope, VarNewConstString("values"), valuesList);
    VarRawSet(funcObj -> scope, VarNewConstString("index"), VarNewNumber(0));
    return func;
}

Var* _pairs(Var* scope, Var* args){
    DebugPrint("_pairs\n");
    Var* pairs = VarRawGet(scope, VarNewConstString("pairs"));
    Var* index = VarRawGet(scope, VarNewConstString("index"));
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
    VarRawSet(scope, VarNewConstString("index"), VarNewNumber(i + 1));
    DebugPrint("_pairs: Sent\n");
    return val;
}

Var* GlobalPairs(Var* scope, Var* args){
    DebugPrint("GlobalPairs\n");
    Var* list = ArgVarGet(args, 0, "obj");
    if(list -> type != VAR_LIST){
        DebugPrint("GlobalPairs: not a list\n");
        return &UNDEFINED;
    }
    DebugPrint("GlobalPairs: %p\n", list);
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
    VarRawSet(funcObj -> scope, VarNewConstString("pairs"), pairsList);
    VarRawSet(funcObj -> scope, VarNewConstString("index"), VarNewNumber(0));
    return func;
}

void PopulateGlobals(Var* globals){
    // Populate the globals with some basic functions.
#define LIBNAME globals
    CONSTANT(pairs, VarNewFunction(GlobalPairs));
    CONSTANT(values, VarNewFunction(GlobalValues));
    CONSTANT(print, VarNewFunction(GlobalPrint));
    CONSTANT(write, VarNewFunction(GlobalWrite));

    // Libs

    // List
    Var* list = VarNewList();
    CONSTANT(list, list);
    PopulateListLib(list);

    // String
    Var* string = VarNewList();
    CONSTANT(string, string);
    PopulateStringLib(string);

    // Math
    Var* math = VarNewList();
    CONSTANT(math, math);
    PopulateMathLib(math);

    // Draw
    Var* draw = VarNewList();
    CONSTANT(draw, draw);
    PopulateDrawLib(draw);

    // OS
    Var* os = VarNewList();
    CONSTANT(os, os);
    PopulateOSLib(os);

    // Meta
    Var* meta = VarNewList();
    CONSTANT(meta, meta);
    PopulateMetaLib(meta);
}

#undef CONSTANT
#undef ALIAS

#endif