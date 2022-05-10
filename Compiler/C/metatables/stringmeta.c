#ifndef STRINGMETA_C
#define STRINGMETA_C

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
    char* newString = calloc(length, sizeof(char));
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
    free(newString);
    return result;
}

void PopulateStringMeta(Var* metatable){
    VarRawSet(metatable, VarNewString("tocode"), VarNewFunction(StringToCode));
}

#endif