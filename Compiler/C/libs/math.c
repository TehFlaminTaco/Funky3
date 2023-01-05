#ifndef MATH_C
#define MATH_C

#include <math.h>
#include <time.h>

#include "lib.h"

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

// Generate a random number.
// If no arguments are supplied, generate a number between [0, 1)
// If one argument is supplied, generate a number between [0,arg)
// If two arguments are supplied, generate a number between [arg, arg2)
// The output is not garunteed to be a whole number.
Var* MathRandom(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "n");
    Var* v2 = ArgVarGet(args, 1, "n2");
    if(v->type != VAR_NUMBER){
        return VarNewNumber(rand() / (double)RAND_MAX);
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    if(v2->type != VAR_NUMBER){j;
        return VarNewNumber(rand() / (double)RAND_MAX * j);
    }
    double j2;
    memcpy(&j2, &v2->value, sizeof(double));
    return VarNewNumber(rand() / (double)RAND_MAX * (j2 - j) + j);
}

// Seed the random number generator
Var* MathRandomSeed(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "n");
    if(v->type != VAR_NUMBER){
        return &NIL;
    }
    double j;
    memcpy(&j, &v->value, sizeof(double));
    srand((int)j);
    return &NIL;
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
    srand (time ( NULL));

    VarRawSet(&MetatableNumber, VarNewConstString("get"), math);

    CONSTANT(abs, VarNewFunction(MathAbs));
    CONSTANT(round, VarNewFunction(MathRound));
    CONSTANT(floor, VarNewFunction(MathFloor));
    CONSTANT(ceil, VarNewFunction(MathCeil));
    CONSTANT(min, VarNewFunction(MathMin));
    CONSTANT(max, VarNewFunction(MathMax));
    CONSTANT(clamp, VarNewFunction(MathClamp));
    CONSTANT(sqrt, VarNewFunction(MathSqrt));
    CONSTANT(rad, VarNewFunction(MathRad));
    CONSTANT(deg, VarNewFunction(MathDeg));
    CONSTANT(random, VarNewFunction(MathRandom));
    CONSTANT(randomSeed, VarNewFunction(MathRandomSeed));

    CONSTANT(pi, VarNewNumber(M_PI));
    CONSTANT(e, VarNewNumber(M_E));
    CONSTANT(huge, VarNewNumber(HUGE_VAL));
    CONSTANT(inf, VarNewNumber(INFINITY));
    CONSTANT(nan, VarNewNumber(NAN));
    CONSTANT(epsilon, VarNewNumber(0.000001));

    CONSTANT(sin, VarNewFunction(MathSin));
    CONSTANT(cos, VarNewFunction(MathCos));
    CONSTANT(tan, VarNewFunction(MathTan));
    CONSTANT(asin, VarNewFunction(MathASin));
    CONSTANT(acos, VarNewFunction(MathACos));
    CONSTANT(atan, VarNewFunction(MathATan));

    ALIAS(abs, a);
    ALIAS(round, r);
    ALIAS(floor, f);
    ALIAS(ceil, F);
    ALIAS(min, x);
    ALIAS(max, X);
    ALIAS(clamp, c);
    ALIAS(sqrt, s);
    ALIAS(rad, d);
    ALIAS(deg, D);
    ALIAS(random, R);
    ALIAS(randomSeed, RS);

    ALIAS(pi, p);
    ALIAS(inf, i);

    ALIAS(sin, S);
    ALIAS(cos, C);
    ALIAS(tan, T);
    ALIAS(asin, as);
    ALIAS(acos, ac);
    ALIAS(atan, at);
}

#endif