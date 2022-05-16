#ifndef BASEMETA_c
#define BASEMETA_c

#include "../funky3.h"
#include "../hashmap.c"

Var* BaseEq(Var* scope, Var* args){
    return VarEquals(ArgVarGet(args, 0, "left"), ArgVarGet(args, 1, "right")) ? VarTrue() : VarFalse();
}

Var* BaseNot(Var* scope, Var* args){
    return (VarTruthy(ArgVarGet(args, 0, "value"))) ? VarFalse() : VarTrue();
}

Var* BaseGe(Var* scope, Var* args){
    // GE is the inverse of LT
    return VarTruthy(VarFunctionCall(VarGetMeta(ArgVarGet(args, 0, "left"), "lt"), args)) ? VarFalse() : VarTrue();
}

Var* BaseLe(Var* scope, Var* args){
    // LE is the inverse of GT
    return VarTruthy(VarFunctionCall(VarGetMeta(ArgVarGet(args, 0, "left"), "gt"), args)) ? VarFalse() : VarTrue();
}

Var* BaseConcat(Var* scope, Var* args){
    Var* left = VarAsString(ArgVarGet(args, 0, "left"));
    Var* right = ArgVarGet(args, 1, "right");

    // Check if the right value has a metamethod, just in case.
    if(right -> type != VAR_STRING){
        DebugPrint("BaseConcat: right not a string\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(!VarTruthy(inverted)){
            Var* rMetaMethod = VarGetMeta(right, "concat");
            if(!ISUNDEFINED(rMetaMethod)){
                Var* nArgs = VarNewList();
                VarRawSet(nArgs, VarNewString("left"), right);
                VarRawSet(nArgs, VarNewString("right"), left);
                VarRawSet(nArgs, VarNewNumber(0), right);
                VarRawSet(nArgs, VarNewNumber(1), left);
                VarRawSet(nArgs, VarNewString("inverted"), VarNewNumber(1));
                VarRawSet(nArgs, VarNewNumber(2), VarNewString("inverted"));
                return VarFunctionCall(rMetaMethod, nArgs);
            }
            return &NIL;
        }
    }
    Var* inverted = ArgVarGet(args, 2, "inverted");
    if(VarTruthy(inverted)){
        Var* t = left;
        left = right;
        right = t;
    }

    right = VarAsString(right);
    int totalSize = strlen(left -> value) + strlen(right -> value) + 1;
    char* result = calloc(totalSize, sizeof(char));
    strcpy(result, left -> value);
    strcat(result, right -> value);
    result[totalSize - 1] = '\0';   // Just in case.
    Var* resultVar = VarNewString(result);
    free(result);
    return resultVar;
}

Var* BaseUnp(Var* scope, Var* args){
    Var* left = ArgVarGet(args, 0, "value");
    if(ISUNDEFINED(left)){
        return &NIL;
    }
    return left;
}

void PopulateBaseMeta(Var* metatable){
    VarRawSet(metatable, VarNewString("eq"), VarNewFunction(BaseEq));
    VarRawSet(metatable, VarNewString("not"), VarNewFunction(BaseNot));
    VarRawSet(metatable, VarNewString("ge"), VarNewFunction(BaseGe));
    VarRawSet(metatable, VarNewString("le"), VarNewFunction(BaseLe));
    VarRawSet(metatable, VarNewString("concat"), VarNewFunction(BaseConcat));
    VarRawSet(metatable, VarNewString("unp"), VarNewFunction(BaseUnp));
}

#endif