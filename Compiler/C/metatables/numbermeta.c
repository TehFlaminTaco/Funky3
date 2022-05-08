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
    char* buffer = calloc(1, sizeof(char) * 32);
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

void PopulateNumberMeta(Var* metatable){
    DebugPrint("!");
    VarRawSet(metatable, VarNewString("tostring"), VarNewFunction(NumberToString));
    VarRawSet(metatable, VarNewString("add"), VarNewFunction(NumberAdd));
    DebugPrint("@");
}

#endif