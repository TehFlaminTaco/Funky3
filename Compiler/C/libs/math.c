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

    if(AB == BC)return B;
    if(BC == CA)return C;
    if(CA == AB)return A;
    return &NIL; // Shouldn't happen, unless someone messed with metatables.
}

Var* MathSqrt(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "n");
    if(v->type != VAR_NUMBER){
        return &NIL;
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    return VarNewNumber(sqrt(j));
}

Var* MathRad(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "n");
    if(v->type != VAR_NUMBER){
        return &NIL;
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    return VarNewNumber(j * M_PI / 180);
}

Var* MathDeg(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "n");
    if(v->type != VAR_NUMBER){
        return &NIL;
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    return VarNewNumber(j * 180 / M_PI);
}


// TRIG
Var* MathSin(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "n");
    if(v->type != VAR_NUMBER){
        return &NIL;
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    return VarNewNumber(sin(j));
}
Var* MathCos(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "n");
    if(v->type != VAR_NUMBER){
        return &NIL;
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    return VarNewNumber(cos(j));
}
Var* MathTan(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "n");
    if(v->type != VAR_NUMBER){
        return &NIL;
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    return VarNewNumber(tan(j));
}
Var* MathASin(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "n");
    if(v->type != VAR_NUMBER){
        return &NIL;
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    return VarNewNumber(asin(j));
}
Var* MathACos(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "n");
    if(v->type != VAR_NUMBER){
        return &NIL;
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    return VarNewNumber(acos(j));
}
Var* MathATan(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "y");
    Var* v2 = ArgVarGet(args, 1, "x");
    if(v->type != VAR_NUMBER){
        return &NIL;
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    if(!ISUNDEFINED(v2)){
        if(v2->type != VAR_NUMBER){
            return &NIL;
        }
        double j2;
        memcpy(&j2, &v2->value, sizeof(double));
        return VarNewNumber(atan2(j, j2));
    }
    return VarNewNumber(atan(j));
}

void PopulateMathLib(Var* math){
    VarRawSet(&MetatableNumber, VarNewString("get"), math);

    VarRawSet(math, VarNewString("abs"), VarNewFunction(MathAbs));
    VarRawSet(math, VarNewString("round"), VarNewFunction(MathRound));
    VarRawSet(math, VarNewString("floor"), VarNewFunction(MathFloor));
    VarRawSet(math, VarNewString("ceil"), VarNewFunction(MathCeil));
    VarRawSet(math, VarNewString("min"), VarNewFunction(MathMin));
    VarRawSet(math, VarNewString("max"), VarNewFunction(MathMax));
    VarRawSet(math, VarNewString("clamp"), VarNewFunction(MathClamp));
    VarRawSet(math, VarNewString("sqrt"), VarNewFunction(MathSqrt));
    VarRawSet(math, VarNewString("rad"), VarNewFunction(MathRad));
    VarRawSet(math, VarNewString("deg"), VarNewFunction(MathDeg));

    VarRawSet(math, VarNewString("pi"), VarNewNumber(M_PI));
    VarRawSet(math, VarNewString("e"), VarNewNumber(M_E));
    VarRawSet(math, VarNewString("huge"), VarNewNumber(HUGE_VAL));
    VarRawSet(math, VarNewString("inf"), VarNewNumber(INFINITY));
    VarRawSet(math, VarNewString("nan"), VarNewNumber(NAN));
    VarRawSet(math, VarNewString("epsilon"), VarNewNumber(0.000001));

    VarRawSet(math, VarNewString("sin"), VarNewFunction(MathSin));
    VarRawSet(math, VarNewString("cos"), VarNewFunction(MathCos));
    VarRawSet(math, VarNewString("tan"), VarNewFunction(MathTan));
    VarRawSet(math, VarNewString("asin"), VarNewFunction(MathASin));
    VarRawSet(math, VarNewString("acos"), VarNewFunction(MathACos));
    VarRawSet(math, VarNewString("atan"), VarNewFunction(MathATan));

}

#endif