#ifndef STRING_C
#define STRING_C

#include "../funky3.h"
#include "../metatable.h"
#include "../libsmallregex.h"

Var* StringMatch(Var* scope, Var* args){
    Var* haystack = VarAsString(ArgVarGet(args, 0, "haystack"));
    Var* needle = VarAsString(ArgVarGet(args, 1, "needle"));
    if(haystack -> type != VAR_STRING || needle -> type != VAR_STRING){
        return &NIL;
    }

    int result = smallregex_match(haystack -> value, needle -> value);
}

void PopulateStringLib(Var* string){
    VarRawSet(&MetatableString, VarNewString("get"), string);

    VarRawSet(string, VarNewString("match"), StringMatch);
}

#endif