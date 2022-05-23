#ifndef STRING_C
#define STRING_C

#include "../funky3.h"
#include "../metatable.h"
#include "../libsmallregex.h"

Var* StringMatch(Var* scope, Var* args){
    Var* haystack = VarAsString(ArgVarGet(args, 0, "haystack"));
    Var* needle = VarAsString(ArgVarGet(args, 1, "needle"));
    Var* offVar = ArgVarGet(args, 2, "offset");
    if(haystack -> type != VAR_STRING || needle -> type != VAR_STRING){
        return &NIL;
    }
    double offD = 0;
    if(offVar -> type == VAR_NUMBER){
        memcpy(&offD, &offVar -> value, sizeof(double));
    }
    int offset = (int)offD;
    if(offset < 0){
        offset = 0;
    }
    int haystackLen = strlen(haystack -> value);
    if(offset >= haystackLen){
        return &NIL;
    }

    match_group_t* groups;
    size_t groupc;
    int result = regex_match(needle -> value, (haystack -> value) + offset, &groupc, &groups);
    if(result < 0){
        free(groups);
        return &NIL;
    }
    if(groupc == 1){ // No groups, return just a string.
        size_t out_len = groups[0].end - groups[0].start;
        char* out = malloc(out_len + 1);
        memcpy(out, haystack -> value + result + offset, out_len);
        out[out_len] = '\0';
        Var* out_var = VarNewString(out);
        free(out);
        return out_var;
    }
    Var* out = VarNewList();
    for(int i=0; i<groupc; i++){
        char* str = calloc(groups[i].end - groups[i].start + 1, sizeof(char));
        strncpy(str, (haystack -> value) + groups[i].start, groups[i].end - groups[i].start);
        str[groups[i].end - groups[i].start] = '\0';
        VarRawSet(out, VarNewNumber(i), VarNewString(str));
        free(str);
    }
    free(groups);
    return out;
}

Var* StringFind(Var* scope, Var* args){
    Var* haystack = VarAsString(ArgVarGet(args, 0, "haystack"));
    Var* needle = VarAsString(ArgVarGet(args, 1, "needle"));
    Var* offVar = ArgVarGet(args, 2, "offset");
    if(haystack -> type != VAR_STRING || needle -> type != VAR_STRING){
        return VarNewNumber(-1);
    }
    double offD = 0;
    if(offVar -> type == VAR_NUMBER){
        memcpy(&offD, &offVar -> value, sizeof(double));
    }
    int offset = (int)offD;
    if(offset < 0){
        offset = 0;
    }
    int haystackLen = strlen(haystack -> value);
    if(offset >= haystackLen){
        return VarNewNumber(-1);
    }

    size_t out_len;
    match_group_t* groups;
    int result = regex_match(needle -> value, (haystack -> value) + offset, &out_len, &groups);
    free(groups);
    if(result < 0){
        return VarNewNumber(-1);
    }
    return VarNewNumber(result + offset);
}

Var* StringSub(Var* scope, Var* args){
    Var* string = VarAsString(ArgVarGet(args, 0, "string"));
    Var* start = ArgVarGet(args, 1, "start");
    Var* length = ArgVarGet(args, 2, "length");
    if(string -> type != VAR_STRING){
        return &NIL;
    }
    if(start -> type != VAR_NUMBER){
        return &NIL;
    }
    int leng = strlen(string -> value);
    if(ISUNDEFINED(length)){
        double j;
        // Copy start-value to j
        memcpy(&j, &start -> value, sizeof(double));
        int startInt = (int)j;
        // if startInt is a negative number, assume we want this many characters from the end
        while(startInt < 0){
            startInt += leng;
        }
        length = VarNewNumber(leng - startInt);
    }
    if(length -> type != VAR_NUMBER){
        return &NIL;
    }

    double startD;
    memcpy(&startD, &start -> value, sizeof(double));
    int startInt = (int)startD;
    double lengthD;
    memcpy(&lengthD, &length -> value, sizeof(double));
    int lengthInt = (int)lengthD;
    if(lengthInt < 0){ // Assume intent is to get the characters from the end
        lengthInt = leng - startInt + lengthInt;
    }
    char* newString = malloc(lengthInt + 1);
    for(int i = 0; i < lengthInt; i++){
        int index = startInt + i;
        // Ensure index lies within the string
        while(index < 0){
            index += leng;
        }
        while(index >= leng){
            index -= leng;
        }
        newString[i] = ((char*)string -> value)[index];
    }
    newString[lengthInt] = '\0';
    Var* out = VarNewString(newString);
    free(newString);
    return out;
}

void PopulateStringLib(Var* string){
    VarRawSet(&MetatableString, VarNewString("get"), string);

    VarRawSet(string, VarNewString("match"), VarNewFunction(StringMatch));
    VarRawSet(string, VarNewString("find"), VarNewFunction(StringFind));
    VarRawSet(string, VarNewString("sub"), VarNewFunction(StringSub));
}

#endif