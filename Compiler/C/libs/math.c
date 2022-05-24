#ifndef MATH_C
#define MATH_C

#include <math.h>

#include "../funky3.h"
#include "../metatable.h"

Var* MathAbs(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "n");
    if(v->type != VAR_NUMBER){
        return &NIL;
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    return VarNewNumber(fabs(j));
}

Var* MathRound(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "n");
    Var* radix = ArgVarGet(args, 1, "radix");
    if(v->type != VAR_NUMBER){
        return &NIL;
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    double r;
    if(radix -> type == VAR_NUMBER){
        memcpy(&r, &radix->value, sizeof(double));
    }else{
        r = 1;
    }
    return VarNewNumber(round(j / r) * r);
}

Var* MathFloor(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "n");
    Var* radix = ArgVarGet(args, 1, "radix");
    if(v->type != VAR_NUMBER){
        return &NIL;
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    double r;
    if(radix -> type == VAR_NUMBER){
        memcpy(&r, &radix->value, sizeof(double));
    }else{
        r = 1;
    }
    return VarNewNumber(floor(j / r) * r);
}

Var* MathCeil(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "n");
    Var* radix = ArgVarGet(args, 1, "radix");
    if(v->type != VAR_NUMBER){
        return &NIL;
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    double r;
    if(radix -> type == VAR_NUMBER){
        memcpy(&r, &radix->value, sizeof(double));
    }else{
        r = 1;
    }
    return VarNewNumber(ceil(j / r) * r);
}

Var* MathMin(Var* scope, Var* args){
    Var* curr = VarRawGet(args, VarNewNumber(0));
    if(ISUNDEFINED(curr)){
        return &NIL;
    }
    Var* right;
    for(int i = 0; !ISUNDEFINED(right=VarRawGet(args, VarNewNumber(i))); i++){
        Var* gt = VarGetMeta(curr, "gt");
        if(gt -> type != VAR_FUNCTION){
            return &NIL;
        }
        Var* cargs = VarNewList();
        ArgVarSet(cargs, 0, "left", curr);
        ArgVarSet(cargs, 1, "right", right);
        if(VarTruthy(VarFunctionCall(gt, cargs))){
            curr = right;
        }   
    }
    return curr;
}

Var* MathMax(Var* scope, Var* args){
    Var* curr = VarRawGet(args, VarNewNumber(0));
    if(ISUNDEFINED(curr)){
        return &NIL;
    }
    Var* right;
    for(int i = 0; !ISUNDEFINED(right=VarRawGet(args, VarNewNumber(i))); i++){
        Var* lt = VarGetMeta(curr, "lt");
        if(lt -> type != VAR_FUNCTION){
            return &NIL;
        }
        Var* cargs = VarNewList();
        ArgVarSet(cargs, 0, "left", curr);
        ArgVarSet(cargs, 1, "right", right);
        if(VarTruthy(VarFunctionCall(lt, cargs))){
            curr = right;
        }   
    }
    return curr;
}

Var* MathClamp(Var* scope, Var* args){
    Var* A = ArgVarGet(args, 0, "A");
    Var* B = ArgVarGet(args, 1, "B");
    Var* C = ArgVarGet(args, 2, "C");

    Var* aLT = VarGetMeta(A, "lt");
    Var* bLT = VarGetMeta(B, "lt");
    Var* cLT = VarGetMeta(C, "lt");

    Var* cArgs = VarNewList();
    ArgVarSet(cArgs, 0, "left", A);
    ArgVarSet(cArgs, 1, "right", B);
    int AB = VarTruthy(VarFunctionCall(aLT, cArgs));
    ArgVarSet(cArgs, 0, "left", B);
    ArgVarSet(cArgs, 1, "right", C);
    int BC = VarTruthy(VarFunctionCall(bLT, cArgs));
    ArgVarSet(cArgs, 0, "left", C);
    ArgVarSet(cArgs, 1, "right", A);
    int CA = VarTruthy(VarFunctionCall(cLT, cArgs));

    if(AB && BC || !AB && !BC)return B;
    if(BC && CA || !BC && !CA)return C;
    if(CA && AB || !CA && !AB)return A;
    return &NIL; // Shouldn't happen, unless someone messed with metatables.
}

void PopulateMathLib(Var* math){
    VarRawSet(math, VarNewString("abs"), VarNewFunction(MathAbs));
    VarRawSet(math, VarNewString("round"), VarNewFunction(MathRound));
    VarRawSet(math, VarNewString("floor"), VarNewFunction(MathFloor));
    VarRawSet(math, VarNewString("ceil"), VarNewFunction(MathCeil));
    VarRawSet(math, VarNewString("min"), VarNewFunction(MathMin));
    VarRawSet(math, VarNewString("max"), VarNewFunction(MathMax));
    VarRawSet(math, VarNewString("clamp"), VarNewFunction(MathClamp));
}

#endif