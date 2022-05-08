#ifndef GLOBALS_C
#define GLOBALS_C

#include <stdio.h>

#include "funky3.h"
#include "var.h"

Var* Print(Var* scope, Var* args){
    DebugPrint("PRINT\n");
    int index = 0;
    Var* arg = ArgVarGet(args, index, NULL);
    if(ISUNDEFINED(arg)){
        DebugPrint("PRINT: No arguments\n");
    }
    while(!ISUNDEFINED(arg)){
        DebugPrint("%i, %i\n", index, arg -> type);
        printf("%s", VarAsString(arg)->value);
        arg = ArgVarGet(args, ++index, NULL);
        if(arg != NULL)
            printf(" ");
    }
    printf("\n");
    return &NIL;
}

void PopulateGlobals(Var* globals){
    // Populate the globals with some basic functions.

    // Define the print function.
    VarRawSet(globals, VarNewString("print"), VarNewFunction(Print));
}

#endif