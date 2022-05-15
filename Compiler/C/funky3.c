#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "funky3.h"

#include "linkedkvlist.h"
#include "linkedlist.h"
#include "var.h"
#include "gc.h"

#include "hashmap.h"
#include "metatable.h"

#include "globals.h"

#include "var.c"
#include "linkedkvlist.c"
#include "linkedlist.c"
#include "gc.c"

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
    gc.toCleanup = LinkedListNew();

    DebugPrint("PreSetup\n");
    SetupMetaTables();

    NIL.metatable = &MetatableNull;
    UNDEFINED.metatable = &MetatableNull;

    Var* scope = VarNewList();
    // Ensure it's a global!!!
    free(scope -> referencedBy);
    scope -> referencedBy = NULL;

    PopulateGlobals(scope);
    DebugPrint("B\n");


    //GarbageCollect();
    Funky3Code(scope);
    //GarbageCollect();
    
    return 0;
}