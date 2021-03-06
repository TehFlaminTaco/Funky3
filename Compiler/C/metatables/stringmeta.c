#ifndef STRINGMETA_C
#define STRINGMETA_C

#include "basemeta.c"
#include "../funky3.h"

// Escape "string" to a code-ready string.
Var* StringToCode(Var* scope, Var* args){
    Var* string = ArgVarGet(args, 0, "obj");
    if(string -> type != VAR_STRING){
        return &UNDEFINED;
    }
    // Calculate the "escaped" string length
    int length = 3; // Minimum of ""\0
    for(char* c = string -> value; *c != '\0'; c++){
        switch(*c){
            // All double character escapes
            case '\a': case '\b': case '\e': case '\f': case '\n': case '\r': case '\t': case '\v': case '\\': case '\'': case '\"':
                length += 2;
                break;
            default:
                if(*c < 32 || *c > 126){
                    length += 6; // \x##
                }else{
                    length++;
                }
                break;
        }
    }
    // Create the new string
    char* newString = tgc_calloc(&gc, length, sizeof(char));
    int index = 0;
    newString[index++] = '"';
    for(char* c = string -> value; *c != '\0'; c++){
        switch(*c){
            case '\a':
                strcpy(newString + index, "\\a");
                index += 2;
                break;
            case '\b':
                strcpy(newString + index, "\\b");
                index += 2;
                break;
            case '\e':
                strcpy(newString + index, "\\e");
                index += 2;
                break;
            case '\f':
                strcpy(newString + index, "\\f");
                index += 2;
                break;
            case '\n':
                strcpy(newString + index, "\\n");
                index += 2;
                break;
            case '\r':
                strcpy(newString + index, "\\r");
                index += 2;
                break;
            case '\t':
                strcpy(newString + index, "\\t");
                index += 2;
                break;
            case '\v':
                strcpy(newString + index, "\\v");
                index += 2;
                break;
            case '\\':
                strcpy(newString + index, "\\\\");
                index += 2;
                break;
            case '\'':
                strcpy(newString + index, "\\'");
                index += 2;
                break;
            case '\"':
                strcpy(newString + index, "\\\"");
                index += 2;
                break;
            default:
                if(*c < 32 || *c > 126){
                    sprintf(newString + index, "\\x%02x", *c);
                    index += 6;
                }else{
                    newString[index] = *c;
                    index++;
                }
                break;
        }
    }
    newString[index++] = '"';
    newString[index] = '\0';
    // Create the new string variable
    Var* result = VarNewString(newString);
    tgc_free(&gc, newString);
    return result;
}

Var* _stringIter(Var* scope, Var* args){
    DebugPrint("_stringIter\n");
    Var* str = VarRawGet(scope, VarNewString("str"));
    Var* index = VarRawGet(scope, VarNewString("index"));
    if(str -> type != VAR_STRING){
        DebugPrint("_stringIter: str is not a string\n");
        return &UNDEFINED;
    }
    if(index -> type != VAR_NUMBER){
        DebugPrint("_stringIter: index is not a number\n");
        return &UNDEFINED;
    }
    char* s = str -> value;
    double j;
    memcpy(&j, &index -> value, sizeof(double));
    int i = (int)j;
    if(s[i] == '\0'){
        DebugPrint("_stringIter: end\n");
        return &UNDEFINED;
    }
    char* oneLetter = tgc_calloc(&gc, 2, sizeof(char));
    oneLetter[0] = s[i];
    oneLetter[1] = '\0';
    DebugPrint("_stringIter: %s\n", oneLetter);
    Var* result = VarNewString(oneLetter);
    tgc_free(&gc, oneLetter);
    VarRawSet(scope, VarNewString("index"), VarNewNumber(i + 1));
    DebugPrint("_stringIter: Sent\n");
    return result;
}

Var* StringIterator(Var* scope, Var* args){
    Var* string = ArgVarGet(args, 0, "obj");
    if(string -> type != VAR_STRING){
        return &UNDEFINED;
    }
    Var* func = VarNewFunction(_stringIter);
    VarFunction* funcObj = (VarFunction*)func -> value;
    funcObj -> scope = VarNewList();
    VarRawSet(funcObj -> scope, VarNewString("str"), string);
    VarRawSet(funcObj -> scope, VarNewString("index"), VarNewNumber(0));
    return func;
}

// Convert a string to a number.
Var* StringUnp(Var* scope, Var* args){
    Var* string = ArgVarGet(args, 0, "value");
    if(string -> type != VAR_STRING){
        return &NIL;
    }
    return VarNewNumber(atof(string -> value));
}

Var* StringLt(Var* scope, Var* args){
    Var* left = ArgVarGet(args, 0, "left");
    Var* right = ArgVarGet(args, 1, "right");
    Var* inverted = ArgVarGet(args, 2, "inverted");
    if(left -> type != VAR_STRING){
        return &NIL;
    }
    if(right -> type != VAR_STRING){
        if(!VarTruthy(inverted)){
            Var* rMetaMethod = VarGetMeta(right, "lt");
            if(ISUNDEFINED(rMetaMethod))return &NIL;
            Var* nArgs = VarNewList();
            ArgVarSet(nArgs, 0, "left", right);
            ArgVarSet(nArgs, 1, "right", left);
            ArgVarSet(nArgs, 2, "inverted", VarTrue());
            return VarFunctionCall(rMetaMethod, nArgs);
        }
        return &NIL;
    }
    return VarNewNumber(strcmp(left -> value, right -> value) < 0);
}

Var* StringGt(Var* scope, Var* args){
    Var* left = ArgVarGet(args, 0, "left");
    Var* right = ArgVarGet(args, 1, "right");
    Var* inverted = ArgVarGet(args, 2, "inverted");
    if(left -> type != VAR_STRING){
        return &NIL;
    }
    if(right -> type != VAR_STRING){
        if(!VarTruthy(inverted)){
            Var* rMetaMethod = VarGetMeta(right, "gt");
            if(ISUNDEFINED(rMetaMethod))return &NIL;
            Var* nArgs = VarNewList();
            ArgVarSet(nArgs, 0, "left", right);
            ArgVarSet(nArgs, 1, "right", left);
            ArgVarSet(nArgs, 2, "inverted", VarTrue());
            return VarFunctionCall(rMetaMethod, nArgs);
        }
        return &NIL;
    }
    return VarNewNumber(strcmp(left -> value, right -> value) > 0);
}

Var* StringLen(Var* scope, Var* args){
    Var* string = ArgVarGet(args, 0, "value");
    if(string -> type != VAR_STRING){
        return &NIL;
    }
    return VarNewNumber(strlen(string -> value));
}

void PopulateStringMeta(Var* metatable){
    VarRawSet(metatable, VarNewString("tocode"), VarNewFunction(StringToCode));
    VarRawSet(metatable, VarNewString("add"), VarNewFunction(BaseConcat));
    VarRawSet(metatable, VarNewString("iter"), VarNewFunction(StringIterator));
    VarRawSet(metatable, VarNewString("unp"), VarNewFunction(StringUnp));
    VarRawSet(metatable, VarNewString("lt"), VarNewFunction(StringLt));
    VarRawSet(metatable, VarNewString("gt"), VarNewFunction(StringGt));
    VarRawSet(metatable, VarNewString("len"), VarNewFunction(StringLen));
}

#endif