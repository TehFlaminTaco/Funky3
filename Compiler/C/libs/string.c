#ifndef STRING_C
#define STRING_C

#include "lib.h"

#include "../funky3.h"
#include "../metatable.h"
#include "../libsmallregex.h"
#include "../linkedlist.h"

#include <ctype.h>

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
    int haystackLen = strlen((char*) haystack -> value);
    if(offset >= haystackLen){
        return &NIL;
    }

    match_group_t* groups;
    size_t groupc;
    int result = regex_match((char*) needle -> value, (haystack -> value) + offset, &groupc, &groups);
    if(result < 0){
        free(groups);
        return &NIL;
    }
    if(groupc == 1){ // No groups, return just a string.
        size_t out_len = groups[0].end - groups[0].start;
        char* out = malloc(out_len + 1);
        memcpy(out, (char*) haystack -> value + result + offset, out_len);
        out[out_len] = '\0';
        Var* out_var = VarNewString(out);
        free(out);
        return out_var;
    }
    Var* out = VarNewList();
    for(int i=0; i<groupc; i++){
        char* str = calloc(groups[i].end - groups[i].start + 1, sizeof(char));
        strncpy(str, ((char*) haystack -> value) + groups[i].start + offset, groups[i].end - groups[i].start);
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
    int haystackLen = strlen((char*) haystack -> value);
    if(offset >= haystackLen){
        return &UNDEFINED;
    }

    match_group_t* groups;
    size_t groupc;
    int result = regex_match((char*) needle -> value, ((char*) haystack -> value) + offset, &groupc, &groups);
    if(result < 0){
        free(groups);
        VarRawSet(scope, VarNewString("offset"), VarNewNumber(haystackLen + 1)); // Set offset to end of string.
        return &UNDEFINED;
    }
    VarRawSet(scope, VarNewString("offset"), VarNewNumber(offset + groups[0].end + 1)); // Set offset to end of match.
    if(groupc == 1){ // No groups, return just a string.
        size_t out_len = groups[0].end - groups[0].start;
        char* out = malloc(out_len + 1);
        memcpy(out, (char*) haystack -> value + result + offset, out_len);
        out[out_len] = '\0';
        Var* out_var = VarNewString(out);
        free(out);
        return out_var;
    }
    Var* out = VarNewList();
    for(int i=0; i<groupc; i++){
        char* str = calloc(groups[i].end - groups[i].start + 1, sizeof(char));
        strncpy(str, ((char*) haystack -> value) + groups[i].start, groups[i].end - groups[i].start);
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
    VarFunction* matchFunc_ = (VarFunction*) matchFunc -> value;
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
    int haystackLen = strlen((char*) haystack -> value);
    if(offset >= haystackLen){
        return VarNewNumber(-1);
    }

    size_t out_len;
    match_group_t* groups;
    int result = regex_match((char*) needle -> value, ((char*) haystack -> value) + offset, &out_len, &groups);
    free(groups);
    if(result < 0){
        return VarNewNumber(-1);
    }
    return VarNewNumber(result + offset);
}

char* _singleReplacement(char* replacement, size_t groupc, char** groups){
    LinkedVarList* combined = LinkedListNew();
    int newStringLength = 0;
    for(char* c = replacement; *c; c++){
        if(*c == '$' && *(c+1) != '\0'){
            c++;
            if(*c >= '0' && *c <= '9'){
                int group = *c - '0';
                if(group < groupc){
                    newStringLength += strlen(groups[group]);
                    LinkedListPush(combined, VarNewString(groups[group]));
                }
            }else{
                newStringLength++;
                char* oneChar = malloc(2);
                oneChar[0] = *c;
                oneChar[1] = '\0';
                LinkedListPush(combined, VarNewString(oneChar));
                free(oneChar);
            }
        }else{
            newStringLength++;
            char* oneChar = malloc(2);
            oneChar[0] = *c;
            oneChar[1] = '\0';
            LinkedListPush(combined, VarNewString(oneChar));
            free(oneChar);
        }
    }

    // Concatenate the strings in reverse order
    newStringLength++;
    char* string = tgc_calloc(&gc, newStringLength, sizeof(char));
    int index = newStringLength-1;
    string[newStringLength] = '\0';
    for(Linklett* current = combined->first; current != NULL; current = current->next){
        char* curString = (char*) current->var->value;
        int curStringLength = strlen(curString);
        index -= curStringLength;
        strncpy(string + index, curString, curStringLength);
    }
    LinkedListFree(combined);
    return string;
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
        if(replacement -> type != VAR_STRING){
            return &NIL;
        }
    }
    double offD = 0;
    if(offVar -> type == VAR_NUMBER){
        memcpy(&offD, &offVar -> value, sizeof(double));
    }
    int offset = (int)offD;
    if(offset < 0){
        offset = 0;
    }
    int haystackLen = strlen((char*) haystack -> value);
    if(offset >= haystackLen){
        return &NIL;
    }
    struct small_regex * reg = regex_compile((char*) needle -> value);
    LinkedVarList* combined = LinkedListNew();
    int lastMatch = 0;
    
    int newStringLength = 0;
    while(1){
        size_t groupc;
        match_group_t* groups;
        int result = regex_match((char*) needle -> value, ((char*) haystack -> value) + offset, &groupc, &groups);
        if(result < 0){
            break;
        }
        result += offset;
        if(result < lastMatch){ // SHOULDN'T HAPPEN
            break;
        }

        // Add the string before the match.
        newStringLength += result - lastMatch;
        char* str = calloc((result - lastMatch) + 1, sizeof(char));
        strncpy(str, ((char*) haystack -> value) + lastMatch, result - lastMatch);
        str[result - lastMatch] = '\0';
        LinkedListPush(combined, VarNewString(str));
        free(str);

        // If the replacement is a function, call it with the match.
        if(replacement -> type == VAR_FUNCTION){
            Var* args = VarNewList();
            for(int i=0; i < groupc; i++){
                size_t match_len = groups[i].end - groups[i].start;
                char* match_str = malloc(match_len + 1);
                memcpy(match_str, (char*) haystack -> value + groups[i].start + result, match_len);
                match_str[match_len] = '\0';
                VarRawSet(args, VarNewNumber(i), VarNewString(match_str));
                free(match_str);
            }
            Var* out = VarFunctionCall(replacement, args);
            if(ISUNDEFINED(out) || ISUNDEFINED(out=VarAsString(out))){
                break;
            }
            newStringLength += strlen((char*) out -> value);
            LinkedListPush(combined, out);
        }
        // Otherwise, Use the replacement string.
        else{
            // Get the full match
            char** groupStrings = calloc(groupc, sizeof(char*));
            for(int i=0; i < groupc; i++){
                size_t match_len = groups[i].end - groups[i].start;
                char* match_str = malloc(match_len + 1);
                memcpy(match_str, (char*) haystack -> value + groups[i].start + result, match_len);
                match_str[match_len] = '\0';
                groupStrings[i] = match_str;
            }
            size_t match_len = groups[0].end - groups[0].start;
            char* match_str = malloc(match_len + 1);
            memcpy(match_str, (char*) haystack -> value + groups[0].start + result, match_len);
            match_str[match_len] = '\0';
            Var* match_var = VarNewString(match_str);
            free(match_str);
            char* repl = _singleReplacement((char*) replacement -> value, groupc, groupStrings);
            for(int i=0; i < groupc; i++){
                free(groupStrings[i]);
            }
            free(groupStrings);
            newStringLength += strlen(repl);
            LinkedListPush(combined, VarNewString(repl));
            free(repl);
        }
        offset = groups[0].end + result;
        lastMatch = offset;
        free(groups);
        if(offset >= haystackLen){
            break;
        }
    }
    newStringLength += haystackLen - lastMatch;
    char* str = calloc((haystackLen - lastMatch) + 1, sizeof(char));
    strncpy(str, ((char*) haystack -> value) + lastMatch, haystackLen - lastMatch);
    str[haystackLen - lastMatch] = '\0';
    LinkedListPush(combined, VarNewString(str));
    free(str);
    regex_free(reg);

    // Concatenate the strings in reverse order
    newStringLength++;
    char* string = tgc_calloc(&gc, newStringLength, sizeof(char));
    int index = newStringLength-1;
    string[newStringLength] = '\0';
    for(Linklett* current = combined->first; current != NULL; current = current->next){
        char* curString = (char*) current->var->value;
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
    int leng = strlen((char*) string -> value);
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

Var* StringReverse(Var* scope, Var* args){
    Var* string = VarAsString(ArgVarGet(args, 0, "string"));
    if(string -> type != VAR_STRING){
        return &NIL;
    }
    int length = strlen((char*) string -> value);
    char* newString = malloc(length + 1);
    for(int i = 0; i < length; i++){
        newString[i] = ((char*)string -> value)[length - i - 1];
    }
    newString[length] = '\0';
    Var* out = VarNewString(newString);
    free(newString);
    return out;
}

Var* StringUpper(Var* scope, Var* args){
    Var* string = VarAsString(ArgVarGet(args, 0, "string"));
    if(string -> type != VAR_STRING){
        return &NIL;
    }
    int length = strlen((char*) string -> value);
    char* newString = malloc(length + 1);
    for(int i = 0; i < length; i++){
        newString[i] = toupper(((char*)string -> value)[i]);
    }
    newString[length] = '\0';
    Var* out = VarNewString(newString);
    free(newString);
    return out;
}

Var* StringLower(Var* scope, Var* args){
    Var* string = VarAsString(ArgVarGet(args, 0, "string"));
    if(string -> type != VAR_STRING){
        return &NIL;
    }
    int length = strlen((char*) string -> value);
    char* newString = malloc(length + 1);
    for(int i = 0; i < length; i++){
        newString[i] = tolower(((char*)string -> value)[i]);
    }
    newString[length] = '\0';
    Var* out = VarNewString(newString);
    free(newString);
    return out;
}

void PopulateStringLib(Var* string){
    VarRawSet(&MetatableString, VarNewString("get"), string);

    CONSTANT(match, VarNewFunction(StringMatch));
    CONSTANT(matches, VarNewFunction(StringMatches));
    CONSTANT(replace, VarNewFunction(StringReplace));
    CONSTANT(find, VarNewFunction(StringFind));
    CONSTANT(sub, VarNewFunction(StringSub));
    CONSTANT(reverse, VarNewFunction(StringReverse));
    CONSTANT(upper, VarNewFunction(StringUpper));
    CONSTANT(lower, VarNewFunction(StringLower));

    ALIAS(match, m);
    ALIAS(matches, M);
    ALIAS(replace, r);
    ALIAS(find, f);
    ALIAS(sub, s);
    ALIAS(reverse, R);
    ALIAS(upper, u);
    ALIAS(lower, l);
}

#endif