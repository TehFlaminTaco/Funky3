#ifndef BASEMETA_c
#define BASEMETA_c

#include "../hashmap.c"

Var* BaseEq(Var* scope, Var* args){
    return VarEquals(ArgVarGet(args, 0, "a"), ArgVarGet(args, 1, "b")) ? VarTrue() : VarFalse();
}

void PopulateBaseMeta(Var* metatable){
    VarRawSet(metatable, VarNewString("eq"), VarNewFunction(BaseEq));
}

#endif