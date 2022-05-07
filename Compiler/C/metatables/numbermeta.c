#ifndef NUMBERMETA_C
#define NUMBERMETA_C

#include <stdio.h>
#include "../funky3.h"
#include "../var.h"
#include "../metatable.h"

Var* NumberToString(Var* scope, Var* args){
    DebugPrint("E\n");
    double j;
    Var* i = ArgVarGet(args, 0, "obj");
    if(i->type != VAR_NUMBER){
        return VarNewString("");
    }
    memcpy(&j, &i->value, sizeof(double));
    char* buffer = calloc(1, sizeof(char) * 32);
    DebugPrint(buffer, 32, "%lf", j);
    // Remove trailing 0s after the decimal point
    char* decimalPoint = strchr(buffer, '.');
    if(decimalPoint != NULL){
        char* end = buffer + strlen(buffer);
        while(end[-1] == '0'){
            end--;
        }
        if(end[-1] == '.'){
            end--;
        }
        *end = '\0';
    }
    return VarNewString(buffer);
}

void PopulateNumberMeta(Var* metatable){
    DebugPrint("!");
    VarRawSet(metatable, VarNewString("tostring"), VarNewFunction(NumberToString));
    DebugPrint("@");
}

#endif