#ifndef GLOBALS_C
#define GLOBALS_C

#include <stdio.h>

#include "funky3.h"
#include "hashmap.h"

#define CONSTANT(name, value) VarRawSet(LIBNAME, VarNewString(#name), value)
#define ALIAS(name, alias) VarRawSet(LIBNAME, VarNewString(#alias), VarRawGet(LIBNAME, VarNewString(#name)))

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

Var* GlobalPrint(Var* scope, Var* args){
    DebugPrint("PRINT\n");
    int index = 0;
    Var* sep = VarRawGet(args, VarNewString("sep"));
    Var* end = VarRawGet(args, VarNewString("end"));
    Var* arg = ArgVarGet(args, index, NULL);
    if(ISUNDEFINED(arg)){
        DebugPrint("PRINT: No arguments\n");
    }
    if(ISUNDEFINED(sep)){
        sep = VarNewString("\t");
    }else{
        sep = VarAsString(sep);
        if(sep -> type != VAR_STRING){
            sep = VarNewString("\t");
        }
    }
    if(ISUNDEFINED(end)){
        end = VarNewString("\n");
    }else{
        end = VarAsString(end);
        if(end -> type != VAR_STRING){
            end = VarNewString("\n");
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
    Var* sep = VarRawGet(args, VarNewString("sep"));
    Var* end = VarRawGet(args, VarNewString("end"));
    Var* arg = ArgVarGet(args, index, NULL);
    if(ISUNDEFINED(arg)){
        DebugPrint("WRITE: No arguments\n");
    }
    if(ISUNDEFINED(sep)){
        sep = VarNewString(" ");
    }else{
        sep = VarAsString(sep);
        if(sep -> type != VAR_STRING){
            sep = VarNewString(" ");
        }
    }
    if(ISUNDEFINED(end)){
        end = VarNewString("");
    }else{
        end = VarAsString(end);
        if(end -> type != VAR_STRING){
            end = VarNewString("");
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
    VarRawSet(funcObj -> scope, VarNewString("pairs"), pairsList);
    VarRawSet(funcObj -> scope, VarNewString("index"), VarNewNumber(0));
    return func;
}

Var* GlobalGetMeta(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "obj");
    return v -> metatable;
}

Var* GlobalSetMeta(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "obj");
    Var* m = ArgVarGet(args, 1, "metatable");
    v -> metatable = m;
    return v -> metatable;
}

Var* GlobalRawGet(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "obj");
    Var* k = ArgVarGet(args, 1, "key");
    return VarRawGet(v, k);
}

Var* GlobalRawSet(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "obj");
    Var* k = ArgVarGet(args, 1, "key");
    Var* v2 = ArgVarGet(args, 2, "value");
    VarRawSet(v, k, v2);
    return v2;
}

Var* GlobalToList(Var* scope, Var* args){
    // Get the iterator of the object and make a list out of it.
    Var* v = ArgVarGet(args, 0, "obj");
    if(v -> type == VAR_LIST){
        return v;
    }
    Var* func;
    Var* nArgs = VarNewList();
    ArgVarSet(nArgs, 0, "obj", v);
    if(v->type == VAR_FUNCTION){
        func = v;
    }else{
        func = VarAsFunction(VarGetMeta(v, "iter"));
        if(func -> type != VAR_FUNCTION){ // Not iteratable.. :(
            return &NIL;
        }
        func = VarFunctionCall(func, nArgs);
    }
    if(func -> type != VAR_FUNCTION){ // Not iteratable.. :(
        return &NIL;
    }
    Var* outList = VarNewList();
    Var* res;
    int index = 0;
    while(!ISUNDEFINED(res = VarFunctionCall(func, nArgs))){
        VarRawSet(outList, VarNewNumber(index++), res);
        ArgVarSet(nArgs, 1, "last", res);
    }
    return outList;
}

void PopulateGlobals(Var* globals){
    // Populate the globals with some basic functions.
#define LIBNAME globals
    CONSTANT(pairs, VarNewFunction(GlobalPairs));
    CONSTANT(values, VarNewFunction(GlobalValues));
    CONSTANT(print, VarNewFunction(GlobalPrint));
    CONSTANT(write, VarNewFunction(GlobalWrite));

    CONSTANT(getMeta, VarNewFunction(GlobalGetMeta));
    CONSTANT(setMeta, VarNewFunction(GlobalSetMeta));

    CONSTANT(rawGet, VarNewFunction(GlobalRawGet));
    CONSTANT(rawSet, VarNewFunction(GlobalRawSet));

    CONSTANT(toList, VarNewFunction(GlobalToList));
    ALIAS(toList, tL);

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
}

#undef CONSTANT
#undef ALIAS

#endif