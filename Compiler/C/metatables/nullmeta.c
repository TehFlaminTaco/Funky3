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
        return VarNewConstString("undefined");
    }
    // nil
    if(null -> value == 0x01){
        DebugPrint("NullToString: nil\n");
        return VarNewConstString("nil");
    }
    // unknown
    DebugPrint("How did we get here?");
    return &UNDEFINED;
}

Var* NullTruthy(Var* scope, Var* args){
    return VarFalse();
}

void PopulateNullMeta(Var* metatable){
    VarRawSet(metatable, VarNewConstString("tostring"), VarNewFunction(NullToString));
    VarRawSet(metatable, VarNewConstString("tocode"),   VarNewFunction(NullToString));
    VarRawSet(metatable, VarNewConstString("truthy"),   VarNewFunction(NullTruthy));
}

#endif