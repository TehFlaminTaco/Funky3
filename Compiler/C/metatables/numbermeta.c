#ifndef NUMBERMETA_C
#define NUMBERMETA_C

#include <stdio.h>
#include "../funky3.h"
#include "../var.h"
#include "../metatable.h"

Var* NumberToString(Var* scope, Var* args){
    DebugPrint("NumberToString\n");
    double j;
    Var* i = ArgVarGet(args, 0, "obj");
    if(i->type != VAR_NUMBER){
        DebugPrint("NumberToString: not a number\n");
        return VarNewString("");
    }
    memcpy(&j, &i->value, sizeof(double));
    char* buffer = tgc_calloc(&gc, 1, sizeof(char) * 32);
    snprintf(buffer, 32, "%lf", j);
    // Remove trailing 0s after the decimal point
    char* decimalPoint = strchr(buffer, '.');
    if(decimalPoint != NULL){
        DebugPrint("NumberToString: Found decimal point\n");
        char* end = buffer + strlen(buffer);
        while(end[-1] == '0'){
            end--;
        }
        if(end[-1] == '.'){
            end--;
        }
        *end = '\0';
    }
    DebugPrint("NumberToString: %s\n", buffer);
    return VarNewString(buffer);
}

Var* NumberAdd(Var* scope, Var* args){
    DebugPrint("NumberAdd\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberAdd: left not a number\n");
        return &NIL;
    }
    Var* right = ArgVarGet(args, 1, "right");
    if(right -> type != VAR_NUMBER){
        DebugPrint("NumberAdd: right not a number\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(VarTruthy(inverted)){
            return &NIL;
        }
        Var* rMetaMethod = VarGetMeta(right, "add");
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
    DebugPrint("NumberAdd: adding\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double k;
    memcpy(&k, &right->value, sizeof(double));
    double result = j + k;
    return VarNewNumber(result);
}

Var* NumberSub(Var* scope, Var* args){
    DebugPrint("NumberSub\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberSub: left not a number\n");
        return &NIL;
    }
    Var* right = ArgVarGet(args, 1, "right");
    if(right -> type != VAR_NUMBER){
        DebugPrint("NumberSub: right not a number\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(VarTruthy(inverted)){
            return &NIL;
        }
        Var* rMetaMethod = VarGetMeta(right, "sub");
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
    DebugPrint("NumberSub: subtracting\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double k;
    memcpy(&k, &right->value, sizeof(double));
    double result = j - k;
    return VarNewNumber(result);
}

Var* NumberMul(Var* scope, Var* args){
    DebugPrint("NumberMul\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberMul: left not a number\n");
        return &NIL;
    }
    Var* right = ArgVarGet(args, 1, "right");
    if(right -> type != VAR_NUMBER){
        DebugPrint("NumberMul: right not a number\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(VarTruthy(inverted)){
            return &NIL;
        }
        Var* rMetaMethod = VarGetMeta(right, "mul");
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
    DebugPrint("NumberMul: multiplying\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double k;
    memcpy(&k, &right->value, sizeof(double));
    double result = j * k;
    return VarNewNumber(result);
}

Var* NumberDiv(Var* scope, Var* args){
    DebugPrint("NumberDiv\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberDiv: left not a number\n");
        return &NIL;
    }
    Var* right = ArgVarGet(args, 1, "right");
    if(right -> type != VAR_NUMBER){
        DebugPrint("NumberDiv: right not a number\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(VarTruthy(inverted)){
            return &NIL;
        }
        Var* rMetaMethod = VarGetMeta(right, "div");
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
    DebugPrint("NumberDiv: dividing\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double k;
    memcpy(&k, &right->value, sizeof(double));
    double result = j / k;
    return VarNewNumber(result);
}

Var* NumberIntDiv(Var* scope, Var* args){
    DebugPrint("NumberIntDiv\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberIntDiv: left not a number\n");
        return &NIL;
    }
    Var* right = ArgVarGet(args, 1, "right");
    if(right -> type != VAR_NUMBER){
        DebugPrint("NumberIntDiv: right not a number\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(VarTruthy(inverted)){
            return &NIL;
        }
        Var* rMetaMethod = VarGetMeta(right, "intdiv");
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
    DebugPrint("NumberIntDiv: dividing\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double k;
    memcpy(&k, &right->value, sizeof(double));
    double result = floor(j / k);
    return VarNewNumber(floor(result));
}

Var* NumberMod(Var* scope, Var* args){
    DebugPrint("NumberMod\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberMod: left not a number\n");
        return &NIL;
    }
    Var* right = ArgVarGet(args, 1, "right");
    if(right -> type != VAR_NUMBER){
        DebugPrint("NumberMod: right not a number\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(VarTruthy(inverted)){
            return &NIL;
        }
        Var* rMetaMethod = VarGetMeta(right, "mod");
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
    DebugPrint("NumberMod: modding\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double k;
    memcpy(&k, &right->value, sizeof(double));
    double result = fmod(j, k);
    return VarNewNumber(result);
}

Var* NumberPow(Var* scope, Var* args){
    DebugPrint("NumberPower\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberPower: left not a number\n");
        return &NIL;
    }
    Var* right = ArgVarGet(args, 1, "right");
    if(right -> type != VAR_NUMBER){
        DebugPrint("NumberPower: right not a number\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(VarTruthy(inverted)){
            return &NIL;
        }
        Var* rMetaMethod = VarGetMeta(right, "power");
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
    DebugPrint("NumberPower: powering\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double k;
    memcpy(&k, &right->value, sizeof(double));
    double result = pow(j, k);
    return VarNewNumber(result);
}

Var* NumberBAnd(Var* scope, Var* args){
    DebugPrint("NumberBAnd\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberBAnd: left not a number\n");
        return &NIL;
    }
    Var* right = ArgVarGet(args, 1, "right");
    if(right -> type != VAR_NUMBER){
        DebugPrint("NumberBAnd: right not a number\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(VarTruthy(inverted)){
            return &NIL;
        }
        Var* rMetaMethod = VarGetMeta(right, "band");
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
    DebugPrint("NumberBAnd: banding\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double k;
    memcpy(&k, &right->value, sizeof(double));
    double result = ((long)floor(j)) & ((long)floor(k));
    return VarNewNumber(result);
}

Var* NumberBOr(Var* scope, Var* args){
    DebugPrint("NumberBOr\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberBOr: left not a number\n");
        return &NIL;
    }
    Var* right = ArgVarGet(args, 1, "right");
    if(right -> type != VAR_NUMBER){
        DebugPrint("NumberBOr: right not a number\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(VarTruthy(inverted)){
            return &NIL;
        }
        Var* rMetaMethod = VarGetMeta(right, "bor");
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
    DebugPrint("NumberBOr: boring\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double k;
    memcpy(&k, &right->value, sizeof(double));
    double result = ((long)floor(j)) | ((long)floor(k));
    return VarNewNumber(result);
}

Var* NumberBXor(Var* scope, Var* args){
    DebugPrint("NumberBXor\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberBXor: left not a number\n");
        return &NIL;
    }
    Var* right = ArgVarGet(args, 1, "right");
    if(right -> type != VAR_NUMBER){
        DebugPrint("NumberBXor: right not a number\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(VarTruthy(inverted)){
            return &NIL;
        }
        Var* rMetaMethod = VarGetMeta(right, "bxor");
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
    DebugPrint("NumberBXor: bxoring\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double k;
    memcpy(&k, &right->value, sizeof(double));
    double result = ((long)floor(j)) ^ ((long)floor(k));
    return VarNewNumber(result);
}

Var* NumberBShL(Var* scope, Var* args){
    DebugPrint("NumberBShL\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberBShL: left not a number\n");
        return &NIL;
    }
    Var* right = ArgVarGet(args, 1, "right");
    if(right -> type != VAR_NUMBER){
        DebugPrint("NumberBShL: right not a number\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(VarTruthy(inverted)){
            return &NIL;
        }
        Var* rMetaMethod = VarGetMeta(right, "bshl");
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
    DebugPrint("NumberBShL: bshling\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double k;
    memcpy(&k, &right->value, sizeof(double));
    double result = ((long)floor(j)) << ((long)floor(k));
    return VarNewNumber(result);
}

Var* NumberBShR(Var* scope, Var* args){
    DebugPrint("NumberBShR\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberBShR: left not a number\n");
        return &NIL;
    }
    Var* right = ArgVarGet(args, 1, "right");
    if(right -> type != VAR_NUMBER){
        DebugPrint("NumberBShR: right not a number\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(VarTruthy(inverted)){
            return &NIL;
        }
        Var* rMetaMethod = VarGetMeta(right, "bshr");
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
    DebugPrint("NumberBShR: bshring\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double k;
    memcpy(&k, &right->value, sizeof(double));
    double result = ((long)floor(j)) >> ((long)floor(k));
    return VarNewNumber(result);
}

Var* NumberGt(Var* scope, Var* args){
    DebugPrint("NumberGt\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberGt: left not a number\n");
        return &NIL;
    }
    Var* right = ArgVarGet(args, 1, "right");
    if(right -> type != VAR_NUMBER){
        DebugPrint("NumberGt: right not a number\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(VarTruthy(inverted)){
            return &NIL;
        }
        Var* rMetaMethod = VarGetMeta(right, "lt");
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
    DebugPrint("NumberGt: comparing\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double k;
    memcpy(&k, &right->value, sizeof(double));
    return j > k ? VarTrue() : VarFalse();
}

Var* NumberLt(Var* scope, Var* args){
    DebugPrint("NumberLt\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberLt: left not a number\n");
        return &NIL;
    }
    Var* right = ArgVarGet(args, 1, "right");
    if(right -> type != VAR_NUMBER){
        DebugPrint("NumberLt: right not a number\n");
        // Call the inversed metamethod... If we need to.
        Var* inverted = ArgVarGet(args, 2, "inverted");
        if(VarTruthy(inverted)){
            return &NIL;
        }
        Var* rMetaMethod = VarGetMeta(right, "lt");
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
    DebugPrint("NumberLt: comparing\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double k;
    memcpy(&k, &right->value, sizeof(double));
    return j < k ? VarTrue() : VarFalse();
}

Var* NumberUnm(Var* scope, Var* args){
    DebugPrint("NumberUnm\n");
    Var* left = ArgVarGet(args, 0, "left");
    if(left -> type != VAR_NUMBER){
        DebugPrint("NumberUnm: left not a number\n");
        return &NIL;
    }
    DebugPrint("NumberUnm: unary minus\n");
    double j;
    memcpy(&j, &left->value, sizeof(double));
    double result = -j;
    return VarNewNumber(result);
}

Var* _numberIter(Var* scope, Var* args){
    DebugPrint("_numberIter\n");
    Var* target = VarRawGet(scope, VarNewString("target"));
    Var* index = VarRawGet(scope, VarNewString("index"));
    if(target -> type != VAR_NUMBER){
        DebugPrint("_stringIter: number is not a number\n");
        return &UNDEFINED;
    }
    if(index -> type != VAR_NUMBER){
        DebugPrint("_stringIter: index is not a number\n");
        return &UNDEFINED;
    }
    double j;
    memcpy(&j, &target->value, sizeof(double));

    double k;
    memcpy(&k, &index->value, sizeof(double));
    if(k >= j){
        return &UNDEFINED;
    }
    VarRawSet(scope, VarNewString("index"), VarNewNumber(k + 1));
    return VarNewNumber(k);
}

Var* NumberIterator(Var* scope, Var* args){
    DebugPrint("NumberIterator\n");
    Var* target = ArgVarGet(args, 0, "target");
    if(target -> type != VAR_NUMBER){
        DebugPrint("NumberIterator: target is not a number\n");
        return &UNDEFINED;
    }
    Var* func = VarNewFunction(_numberIter);
    VarFunction* funcObj = (VarFunction*)func -> value;
    funcObj -> scope = VarNewList();
    VarRawSet(funcObj -> scope, VarNewString("target"), target);
    VarRawSet(funcObj -> scope, VarNewString("index"), VarNewNumber(0));
    return func;
}

void PopulateNumberMeta(Var* metatable){
    DebugPrint("!");
    VarRawSet(metatable, VarNewString("tostring"), VarNewFunction(NumberToString));
    VarRawSet(metatable, VarNewString("tocode"),   VarNewFunction(NumberToString));
    VarRawSet(metatable, VarNewString("add"), VarNewFunction(NumberAdd));
    VarRawSet(metatable, VarNewString("sub"), VarNewFunction(NumberSub));
    VarRawSet(metatable, VarNewString("mul"), VarNewFunction(NumberMul));
    VarRawSet(metatable, VarNewString("div"), VarNewFunction(NumberDiv));
    VarRawSet(metatable, VarNewString("intdiv"), VarNewFunction(NumberIntDiv));
    VarRawSet(metatable, VarNewString("mod"), VarNewFunction(NumberMod));
    VarRawSet(metatable, VarNewString("pow"), VarNewFunction(NumberPow));
    VarRawSet(metatable, VarNewString("band"), VarNewFunction(NumberBAnd));
    VarRawSet(metatable, VarNewString("bor"), VarNewFunction(NumberBOr));
    VarRawSet(metatable, VarNewString("bxor"), VarNewFunction(NumberBXor));
    VarRawSet(metatable, VarNewString("bshr"), VarNewFunction(NumberBShR));
    VarRawSet(metatable, VarNewString("bshl"), VarNewFunction(NumberBShL));

    VarRawSet(metatable, VarNewString("unm"), VarNewFunction(NumberUnm));

    VarRawSet(metatable, VarNewString("gt"), VarNewFunction(NumberGt));
    VarRawSet(metatable, VarNewString("lt"), VarNewFunction(NumberLt));

    VarRawSet(metatable, VarNewString("iter"), VarNewFunction(NumberIterator));
    DebugPrint("@");
}

#endif