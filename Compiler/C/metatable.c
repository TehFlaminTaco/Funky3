#ifndef METATABLE_C
#define METATABLE_C

#include "funky3.h"
#include "metatable.h"

#include "hashmap.h"

#include "metatables/basemeta.c"
#include "metatables/functionmeta.c"
#include "metatables/listmeta.c"
#include "metatables/nullmeta.c"
#include "metatables/numbermeta.c"
#include "metatables/stringmeta.c"
#include "metatables/withmeta.c"

void SetupMetaTables(){
    // Initialize metatables with new HashMaps.
    MetatableBase.value = (long long)HashMapNew(59);
    tgc_set_flags(&gc, MetatableBase.value, TGC_ROOT);
    MetatableBase.metatable = &MetatableList;
    PopulateBaseMeta(&MetatableBase);

    MetatableNull.value = (long long)HashMapNew(59);
    tgc_set_flags(&gc, MetatableNull.value, TGC_ROOT);
    MetatableNull.metatable = &MetatableList;
    PopulateNullMeta(&MetatableNull);

    MetatableNumber.value = (long long)HashMapNew(59);
    tgc_set_flags(&gc, MetatableNumber.value, TGC_ROOT);
    MetatableNumber.metatable = &MetatableList;
    PopulateNumberMeta(&MetatableNumber);

    MetatableString.value = (long long)HashMapNew(59);
    tgc_set_flags(&gc, MetatableString.value, TGC_ROOT);
    MetatableString.metatable = &MetatableList;
    PopulateStringMeta(&MetatableString);

    MetatableFunction.value = (long long)HashMapNew(59);
    tgc_set_flags(&gc, MetatableFunction.value, TGC_ROOT);
    MetatableFunction.metatable = &MetatableList;
    PopulateFunctionMeta(&MetatableFunction);

    MetatableList.value = (long long)HashMapNew(59);
    tgc_set_flags(&gc, MetatableList.value, TGC_ROOT);
    MetatableList.metatable = &MetatableList;
    PopulateListMeta(&MetatableList);

    MetatableWith.value = (long long)HashMapNew(59);
    tgc_set_flags(&gc, MetatableWith.value, TGC_ROOT);
    MetatableWith.metatable = &MetatableList;
    PopulateListMeta(&MetatableWith);
    PopulateWithMeta(&MetatableWith);
}

Var* VarGetMeta(Var* var, char* key){
    DebugPrint("VarGetMeta: %s\n", key);
    Var* keyVar = VarNewString(key);
    DebugPrint("VarGetMeta: Made Key\n");
    if(var->metatable == NULL){
        DebugPrint("VarGetMeta: No metatable\n");
        return VarRawGet(&MetatableBase, keyVar);
    }
    // Assert the Var's metatable is a list
    if(var->metatable->type != 0x04){
        DebugPrint("VarGetMeta: Assertion failed: var->metatable->type != 0x04 (was 0x%02x)\n", var->metatable->type);
        return VarRawGet(&MetatableBase, keyVar);
    }
    DebugPrint("VarGetMeta: Assertion passed\n");

    // Search the var's metatable for the key
    // If the key is not found, return NIL.
    if((HashMap*)var -> metatable -> value == NULL){
        DebugPrint("VarGetMeta: Assertion failed: var -> metatable -> value == NULL\n");
        return VarRawGet(&MetatableBase, keyVar);
    }
    DebugPrint("VarGetMeta: Assertion passed\n");
    HashMap* metatable = (HashMap*)(intptr_t)var->metatable->value;
    DebugPrint("VarGetMeta: metatable: %p\n", metatable);
    Var* meta = HashMapGet(metatable, keyVar);
    if(ISUNDEFINED(meta)){
        DebugPrint("VarGetMeta: Key not found.\n");
        return VarRawGet(&MetatableBase, keyVar);
    }

    DebugPrint("VarGetMeta: Key found.\n");
    return meta;
}

#endif