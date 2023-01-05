#ifndef BASEMETA_c
#define BASEMETA_c

#include "../funky3.h"
#include "../hashmap.c"

Var* BaseTruthy(Var* scope, Var* args){
    return VarTrue();
}

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

Var* BaseTryRight(Var* scope, Var* args){
    Var* left = ArgVarGet(args, 0, "left");
    Var* right = ArgVarGet(args, 1, "right");
    Var* inverted = ArgVarGet(args, 2, "inverted");

    Var* metaMethod = VarRawGet(scope, VarNewConstString("metamethod"));
    if(ISUNDEFINED(metaMethod) || metaMethod -> type != VAR_STRING || VarTruthy(inverted)){
        return &NIL;
    }
    Var* nArgs = VarNewList();
    ArgVarSet(nArgs, 0, "left", right);
    ArgVarSet(nArgs, 1, "right", left);
    ArgVarSet(nArgs, 2, "inverted", VarTrue());
    return VarFunctionCall(VarGetMeta(right, (char*) metaMethod -> value), nArgs);
}

void GenerateTryRight(Var* metatable, char* metamethod){
    Var* tryRight = VarNewFunction(BaseTryRight);
    VarFunction* func = (VarFunction*)tryRight -> value;
    func -> scope = VarNewList();
    Var* metaMethod = VarNewString(metamethod);
    VarRawSet(func -> scope, VarNewConstString("metamethod"), metaMethod);
    VarRawSet(metatable, metaMethod, tryRight);
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
                VarRawSet(nArgs, VarNewConstString("left"), right);
                VarRawSet(nArgs, VarNewConstString("right"), left);
                VarRawSet(nArgs, VarNewNumber(0), right);
                VarRawSet(nArgs, VarNewNumber(1), left);
                VarRawSet(nArgs, VarNewConstString("inverted"), VarNewNumber(1));
                VarRawSet(nArgs, VarNewNumber(2), VarNewConstString("inverted"));
                return VarFunctionCall(rMetaMethod, nArgs);
            }
            return &NIL;
        }else{
            right = VarAsString(right);
        }
    }
    Var* inverted = ArgVarGet(args, 2, "inverted");
    if(VarTruthy(inverted)){
        Var* t = left;
        left = right;
        right = t;
    }

    right = VarAsString(right);
    int totalSize = strlen((char*) left -> value) + strlen((char*) right -> value) + 1;
    char* result = tgc_calloc(&gc, totalSize, sizeof(char));
    strcpy(result, (char*) left -> value);
    strcat(result, (char*) right -> value);
    result[totalSize - 1] = '\0';   // Just in case.
    Var* resultVar = VarNewString(result);
    tgc_free(&gc, result);
    return resultVar;
}

Var* BaseUnp(Var* scope, Var* args){
    Var* left = ArgVarGet(args, 0, "value");
    if(ISUNDEFINED(left)){
        return &NIL;
    }
    return left;
}

Var* BaseAnd(Var* scope, Var* args){
    Var* left = ArgVarGet(args, 0, "left");
    Var* right = ArgVarGet(args, 1, "right");
    if(VarTruthy(left)){
        return right;
    }
    return left;
}

Var* BaseOr(Var* scope, Var* args){
    Var* left = ArgVarGet(args, 0, "left");
    Var* right = ArgVarGet(args, 1, "right");
    if(VarTruthy(left)){
        return left;
    }
    return right;
}

Var* BaseDispose(Var* scope, Var* args){
    Var* obj = ArgVarGet(args, 0, "obj");
    return VarFunctionCall(VarRawGet(obj, VarNewConstString("dispose")), args);
}

void PopulateBaseMeta(Var* metatable){
    VarRawSet(metatable, VarNewConstString("dispose"), VarNewFunction(BaseDispose));
    VarRawSet(metatable, VarNewConstString("truthy"), VarNewFunction(BaseTruthy));
    VarRawSet(metatable, VarNewConstString("eq"), VarNewFunction(BaseEq));
    VarRawSet(metatable, VarNewConstString("not"), VarNewFunction(BaseNot));
    VarRawSet(metatable, VarNewConstString("ge"), VarNewFunction(BaseGe));
    VarRawSet(metatable, VarNewConstString("le"), VarNewFunction(BaseLe));
    VarRawSet(metatable, VarNewConstString("concat"), VarNewFunction(BaseConcat));
    VarRawSet(metatable, VarNewConstString("unp"), VarNewFunction(BaseUnp));

    GenerateTryRight(metatable, "add");
    GenerateTryRight(metatable, "sub");
    GenerateTryRight(metatable, "mul");
    GenerateTryRight(metatable, "div");
    GenerateTryRight(metatable, "intdiv");
    GenerateTryRight(metatable, "mod");
    GenerateTryRight(metatable, "pow");
    GenerateTryRight(metatable, "band");
    GenerateTryRight(metatable, "bor");
    GenerateTryRight(metatable, "bxor");
    GenerateTryRight(metatable, "bshl");
    GenerateTryRight(metatable, "bshr");
    GenerateTryRight(metatable, "gt");
    GenerateTryRight(metatable, "lt");
    
    GenerateTryRight(metatable, "and");
    GenerateTryRight(metatable, "or");
}

#endif