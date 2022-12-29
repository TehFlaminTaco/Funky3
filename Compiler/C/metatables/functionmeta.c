#ifndef FUNCTIONMETA_C
#define FUNCTIONMETA_C

#include "../funky3.h"

Var* FunctionAsString(Var* scope, Var* args){
    Var* function = ArgVarGet(args, 0, "obj");
    if(function -> type != VAR_FUNCTION){
        return &UNDEFINED;
    }
    VarFunction* func = (VarFunction*)function -> value;
    return VarNewString(func -> name);
}

void PopulateFunctionMeta(Var* metatable){
    VarRawSet(metatable, VarNewString("tostring"), VarNewFunction(FunctionAsString));
}

#endif