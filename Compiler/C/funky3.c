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

#include "var.c"
#include "linkedkvlist.c"
#include "linkedlist.c"
#include "gc.c"

#include "hashmap.c"
#include "metatable.c"

#include "header.c"

int main(int argc, char** argv){
    gc.toCleanup = LinkedListNew();

    DebugPrint("PreSetup\n");
    SetupMetaTables();
    DebugPrint("A\n");

    Var* var = VarNewNumber(145.82);
    Var* numAsString = VarAsString(var);
    DebugPrint("%s\n", (char*)numAsString->value);
    DebugPrint("B\n");

    GarbageCollect();

#include "main.c"

    GarbageCollect();
    
    return 0;
}