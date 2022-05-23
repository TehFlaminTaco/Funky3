#ifndef STRING_C
#define STRING_C

#include "../funky3.h"
#include "../metatable.h"
#include "../libsmallregex.h"
#include "../linkedlist.h"

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
        strncpy(str, (haystack -> value) + groups[i].start + offset, groups[i].end - groups[i].start);
        str[groups[i].end - groups[i].start] = '\0';
        VarRawSet(out, VarNewNumber(i), VarNewString(str));
        free(str);
    }
    free(groups);
    return out;
}

Var* _stringMatches(Var* scope, Var* args){
    Var* haystack = VarAsString(VarRawGet(scope, VarNewString("haystack")));
    Var* needle = VarAsString(VarRawGet(scope, VarNewString("needle")));
    Var* offVar = VarRawGet(scope, VarNewString("offset"));
    if(haystack -> type != VAR_STRING || needle -> type != VAR_STRING){
        return &UNDEFINED;
    }
    if(offVar -> type != VAR_NUMBER){
        return &UNDEFINED;
    }
    double offD = 0;
    memcpy(&offD, &offVar -> value, sizeof(double));
    int offset = (int)offD;
    if(offset < 0){
        offset = 0;
    }
    int haystackLen = strlen(haystack -> value);
    if(offset >= haystackLen){
        return &UNDEFINED;
    }

    match_group_t* groups;
    size_t groupc;
    int result = regex_match(needle -> value, (haystack -> value) + offset, &groupc, &groups);
    if(result < 0){
        free(groups);
        VarRawSet(scope, VarNewString("offset"), VarNewNumber(haystackLen + 1)); // Set offset to end of string.
        return &UNDEFINED;
    }
    VarRawSet(scope, VarNewString("offset"), VarNewNumber(offset + groups[0].end + 1)); // Set offset to end of match.
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

Var* StringMatches(Var* scope, Var* args){
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

    Var* matchFunc = VarNewFunction(_stringMatches);
    VarFunction* matchFunc_ = matchFunc -> value;
    matchFunc_ -> scope = VarNewList();
    VarRawSet(matchFunc_ -> scope, VarNewString("haystack"), haystack);
    VarRawSet(matchFunc_ -> scope, VarNewString("needle"), needle);
    VarRawSet(matchFunc_ -> scope, VarNewString("offset"), VarNewNumber(offset));
    return matchFunc;
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

Var* StringReplace(Var* scope, Var* args){
    Var* haystack = VarAsString(ArgVarGet(args, 0, "haystack"));
    Var* needle = VarAsString(ArgVarGet(args, 1, "needle"));
    Var* replacement = ArgVarGet(args, 2, "replacement");
    Var* offVar = ArgVarGet(args, 3, "offset");
    if(haystack -> type != VAR_STRING || needle -> type != VAR_STRING){
        return &NIL;
    }
    if(replacement -> type != VAR_FUNCTION){
        replacement = VarAsString(replacement);
    }
    if(replacement -> type != VAR_STRING){
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
    struct small_regex * reg = regex_compile(needle -> value);
    LinkedVarList* combined = LinkedListNew();
    int lastMatch = 0;
    
    int newStringLength = 0;
    while(1){
        size_t groupc;
        match_group_t* groups;
        int result = regex_match(needle -> value, (haystack -> value) + offset, &groupc, &groups);
        if(result < 0){
            break;
        }
        result += offset;
        if(result < lastMatch){ // SHOULDN'T HAPPEN
            break;
        }

        // Add the string before the match.
        newStringLength += result - lastMatch;
        char* str = calloc(result - lastMatch, sizeof(char));
        strncpy(str, (haystack -> value) + lastMatch, result - lastMatch);
        str[result - lastMatch] = '\0';
        LinkedListInsert(combined, VarNewString(str), 0);

        // If the replacement is a function, call it with the match.
        if(replacement -> type == VAR_FUNCTION){
            Var* args = VarNewList();
            for(int i=0; i < groupc; i++){
                size_t match_len = groups[i].end - groups[i].start;
                char* match_str = malloc(match_len + 1);
                memcpy(match_str, haystack -> value + result + offset, match_len);
                match_str[match_len] = '\0';
                VarRawSet(args, VarNewNumber(i), VarNewString(match_str));
                free(match_str);
            }
            Var* out = VarFunctionCall(replacement, args);
            if(ISUNDEFINED(out) || ISUNDEFINED(out=VarAsString(out))){
                break;
            }
            newStringLength += strlen(out -> value);
            LinkedListInsert(combined, out, 0);
        }
        // Otherwise, Use the replacement string.
        else{
            // Get the full match
            size_t match_len = groups[0].end - groups[0].start;
            char* match_str = malloc(match_len + 1);
            memcpy(match_str, haystack -> value + result + offset, match_len);
            match_str[match_len] = '\0';
            Var* match_var = VarNewString(match_str);
            printf("??: %s\n", match_str);
            free(match_str);
            newStringLength += strlen(replacement -> value);
            LinkedListInsert(combined, replacement, 0);
        }

        offset += groups[0].end + 1;
        free(groups);
        regex_free(reg);
    }

    // Concatenate the strings in reverse order
    newStringLength++;
    char* string = tgc_calloc(&gc, newStringLength, sizeof(char));
    int index = newStringLength;
    string[newStringLength] = '\0';
    for(Linklett* current = combined->first; current != NULL; current = current->next){
        char* curString = current->var->value;
        int curStringLength = strlen(curString);
        index -= curStringLength;
        strncpy(string + index, curString, curStringLength);
    }
    LinkedListFree(combined);
    Var* result = VarNewString(string);
    tgc_free(&gc, string);
    return result;
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
    VarRawSet(string, VarNewString("matches"), VarNewFunction(StringMatches));
    VarRawSet(string, VarNewString("replace"), VarNewFunction(StringReplace));
    VarRawSet(string, VarNewString("find"), VarNewFunction(StringFind));
    VarRawSet(string, VarNewString("sub"), VarNewFunction(StringSub));
}

#endif