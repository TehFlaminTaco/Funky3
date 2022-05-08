#ifndef METATABLE_C
#define METATABLE_C

#include "funky3.h"
#include "metatable.h"

#include "hashmap.h"

#include "metatables/basemeta.c"
#include "metatables/numbermeta.c"

void SetupMetaTables(){
    // Initialize metatables with new HashMaps.
    MetatableNull.value = (long long)HashMapNew(16);
    MetatableNull.metatable = &MetatableList;
    PopulateBaseMeta(&MetatableNull);

    MetatableNumber.value = (long long)HashMapNew(16);
    MetatableNumber.metatable = &MetatableList;
    PopulateBaseMeta(&MetatableNumber);
    PopulateNumberMeta(&MetatableNumber);

    MetatableString.value = (long long)HashMapNew(16);
    MetatableString.metatable = &MetatableList;
    PopulateBaseMeta(&MetatableString);

    MetatableFunction.value = (long long)HashMapNew(16);
    MetatableFunction.metatable = &MetatableList;
    PopulateBaseMeta(&MetatableFunction);

    MetatableList.value = (long long)HashMapNew(16);
    MetatableList.metatable = &MetatableList;
    PopulateBaseMeta(&MetatableList);
}

Var* VarGetMeta(Var* var, char* key){
    DebugPrint("VarGetMeta: %s\n", key);
    Var* keyVar = VarNewString(key);
    DebugPrint("VarGetMeta: Made Key\n");
    if(var->metatable == NULL){
        DebugPrint("VarGetMeta: No metatable\n");
        return NULL;
    }
    // Assert the Var's metatable is a list
    if(var->metatable->type != 0x04){
        DebugPrint("VarGetMeta: Assertion failed: var->metatable->type != 0x04\n");
        return &UNDEFINED;
    }
    DebugPrint("VarGetMeta: Assertion passed\n");

    // Search the var's metatable for the key
    // If the key is not found, return NIL.
    if(var -> metatable -> value == NULL){
        DebugPrint("VarGetMeta: Assertion failed: var -> metatable -> value == NULL\n");
        return &UNDEFINED;
    }
    DebugPrint("VarGetMeta: Assertion passed\n");
    HashMap* metatable = (HashMap*)(intptr_t)var->metatable->value;
    DebugPrint("VarGetMeta: metatable: %p\n", metatable);
    Var* meta = HashMapGet(metatable, keyVar);
    if(meta == NULL){
        DebugPrint("VarGetMeta: Key not found.\n");
        return &UNDEFINED;
    }

    DebugPrint("VarGetMeta: Key found.\n");
    return meta;
}

#endif