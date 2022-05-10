#ifndef NULLMETA_C
#define NULLMETA_C

#include "../funky3.h"
#include "../var.h"

Var* NullToString(Var* scope, Var* args){
    DebugPrint("NullToString\n");
    Var* null = ArgVarGet(args, 0, "obj");
    if(null -> type != VAR_NULL){
        DebugPrint("NullToString: not null\n");
        return &UNDEFINED;
    }
    // undefined
    if(null -> value == 0x00){
        DebugPrint("NullToString: undefined\n");
        return VarNewString("undefined");
    }
    // nil
    if(null -> value == 0x01){
        DebugPrint("NullToString: nil\n");
        return VarNewString("nil");
    }
    // unknown
    DebugPrint("How did we get here?");
    return &UNDEFINED;
}

void PopulateNullMeta(Var* metatable){
    VarRawSet(metatable, VarNewString("tostring"), VarNewFunction(NullToString));
    VarRawSet(metatable, VarNewString("tocode"),   VarNewFunction(NullToString));
}

#endif