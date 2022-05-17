#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "funky3.h"

#include "tgc.c"

#include "linkedkvlist.h"
#include "linkedlist.h"
#include "var.h"

#include "hashmap.h"
#include "metatable.h"

#include "globals.h"

#include "var.c"
#include "linkedkvlist.c"
#include "linkedlist.c"

#include "hashmap.c"
#include "metatable.c"

#include "globals.c"

#include "header.c"

Var* Funky3Code(Var* scope){
    Var* _;
    DebugPrint("USER GENERATED CODE:\n");
    #include "main.c"
    DebugPrint("USER GENERATED CODE END\n");
}

int main(int argc, char** argv){
    tgc_start(&gc, &argc);
    tgc_pause(&gc);

    DebugPrint("PreSetup\n");
    SetupMetaTables();

    NIL.metatable = &MetatableNull;
    UNDEFINED.metatable = &MetatableNull;

    Var* scope = VarNewList();
    tgc_set_flags(&gc, scope, TGC_ROOT);
    PopulateGlobals(scope);
    DebugPrint("B\n");
    tgc_run(&gc);
    Funky3Code(scope);
    
    tgc_stop(&gc);
    return 0;
}