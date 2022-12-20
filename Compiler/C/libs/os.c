
#ifndef OS_C
#define OS_C

#include "lib.h"
#include <time.h>

#include "../funky3.h"
#include "../metatable.h"
#include "../hashmap.h"

Var* OSClock(Var* scope, Var* args){
    return VarNewNumber((double)(clock())/CLOCKS_PER_SEC);
}

void PopulateOSLib(Var* os){
    CONSTANT(clock, VarNewFunction(OSClock));
    ALIAS(clock, c);
}

#endif