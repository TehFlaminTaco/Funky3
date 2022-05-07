
#ifndef VAR_C
#define VAR_C

#include <stdio.h>

#include "funky3.h"

#include "var.h"
#include "hashmap.h"
#include "metatable.h"

#include "linkedlist.h"

Var* VarNew(char type, long long value, Var* metatable){
    Var* var = calloc(1, sizeof(Var));
    var->type = type;
    var->value = value;
    var->metatable = metatable;
    var->referencedBy = LinkedListNew();
    var->gcMark = 0;
    return var;
}

void FreeVar(Var* var){
    if(var->type == VAR_STRING || var->type == VAR_FUNCTION || var->type == VAR_LIST){
        free((void*)var->value);
    }
    if(var->referencedBy != NULL)
        LinkedListFree(var->referencedBy);
    if(var->metatable != NULL)
        DerefferenceVar(var->metatable, var);
    free(var);
}

Var* VarNewNumber(double value){
    long long j;
    memcpy(&j, &value, sizeof(double));
    return VarFreeLater(VarNew(VAR_NUMBER, j, &MetatableNumber));
}

Var* VarNewString(char* value){
    char* copiedString = calloc(strlen(value) + 1, sizeof(char));
    strcpy(copiedString, value);
    return VarFreeLater(VarNew(VAR_STRING, (long long)copiedString, &MetatableString));
}

Var* VarNewList(){
    return VarFreeLater(VarNew(VAR_LIST, (long long)HashMapNew(256), &MetatableList));
}

Var* VarNewFunction(Var* (value)(Var*, Var*)){
    VarFunction* func = calloc(1, sizeof(VarFunction));
    func->method = value;
    func->scope = &NIL;
    func->name = NULL;
    return VarFreeLater(VarNew(VAR_FUNCTION, (long long)func, &MetatableFunction));
}

Var* VarTrue(){
    return VarNewNumber(1.0);
}

Var* VarFalse(){
    return VarNewNumber(0.0);
}


int VarEquals(Var* var1, Var* var2){
    if(var1->type != var2->type){
        return 0;
    }

    switch(var1->type){
        case VAR_NULL: // NIL
            return var1->value == var2->value;
        case VAR_NUMBER: // double.
            double a;
            double b;
            memcpy(&a, &var1->value, sizeof(double));
            memcpy(&b, &var2->value, sizeof(double));
            return fabs(a-b) < 0.00001;
        case VAR_STRING: // String
            return strcmp((char*)(intptr_t)var1->value, (char*)(intptr_t)var2->value) == 0;
            //return (char*)(intptr_t)var2->value == NULL;
        

        // Function, Object
        case VAR_FUNCTION: case VAR_LIST:
            return var1->value == var2->value;
        
        default:
            return 0;
    }
}

Var* VarFunctionCall(Var* function, Var* args){
    DebugPrint("VarFunctionCall\n");
    if(function->type != VAR_FUNCTION){
        DebugPrint("VarFunctionCall: Not a function\n");
        return &NIL;
    }

    VarFunction* func = (VarFunction*)function->value;
    DebugPrint("VarFunctionCall: Calling function %p\n", func->method);
    return (func->method)(func->scope, args);
}

Var* VarRawGet(Var* table, Var* key){
    DebugPrint("VarRawGet\n");
    if(table->type != VAR_LIST){
        DebugPrint("VarRawGet: table is not a list\n");
        return &NIL;
    }

    DebugPrint("VarRawGet: table is a list\n");
    HashMap* map = (HashMap*)table->value;
    DebugPrint("VarRawGet: map is %p\n", map);
    return HashMapGet(map, key);
}

Var* VarRawSet(Var* table, Var* key, Var* value){
    DebugPrint("VarRawSet\n");
    if(table->type != VAR_LIST){
        DebugPrint("VarRawSet: table is not a list\n");
        return &NIL;
    }
    // Remove any currently set var from the referencedby list.
    DebugPrint("VarRawSet: removing old value\n");
    Var* oldValue = VarRawGet(table, key);
    DebugPrint("VarRawSet: old value is %p\n", oldValue);
    if(oldValue->referencedBy != NULL){
        DebugPrint("VarRawSet: removing old value from referencedby list\n");
        LinkedListRemoveByValue(oldValue->referencedBy, table);
    }
    // Add to the referencedby list
    DebugPrint("VarRawSet: adding new value to referencedby list\n");
    if(value->referencedBy != NULL){
        DebugPrint("VarRawSet: adding new value to referencedby list\n");
        LinkedListPush(value->referencedBy, table);
    }
    DebugPrint("VarRawSet: setting value\n");
    HashMap* map = (HashMap*)table->value;
    HashMapSet(map, key, value);
    DebugPrint("VarRawSet: done\n");
    return value;
}

Var* VarGet(Var* table, Var* key){
    DebugPrint("VarGet\n");
    // Try Rawget first
    Var* value = VarRawGet(table, key);
    if(!(value == NULL || (value -> type == VAR_NULL && value -> value == 0))){
        return value;
    }
    // Use the "get" metamethod
    Var* getter = VarGetMeta(table, "get");
    if(getter->type != VAR_FUNCTION){
        DebugPrint("VarGet: No getter\n");
        return &NIL;
    }
    // Call the getter
    Var* args = VarNewList();
    VarRawSet(args, VarNewString("table"), table);
    VarRawSet(args, VarNewString("key"), key);
    VarRawSet(args, VarNewNumber(0), table);
    VarRawSet(args, VarNewNumber(1), key);
    return VarFunctionCall(getter, args);
}

Var* VarSet(Var* table, Var* key, Var* value){
    DebugPrint("VarSet\n");
    // Is there anything defined?
    Var* oldValue = VarGet(table, key);
    if(oldValue->type == VAR_NULL && oldValue->value == 0){
        // Use the "set" metamethod
        Var* setter = VarGetMeta(table, "set");
        if(setter->type == VAR_FUNCTION){
            Var* args = VarNewList();
            VarRawSet(args, VarNewString("table"), table);
            VarRawSet(args, VarNewString("key"), key);
            VarRawSet(args, VarNewString("value"), value);
            VarRawSet(args, VarNewNumber(0), table);
            VarRawSet(args, VarNewNumber(1), key);
            VarRawSet(args, VarNewNumber(2), value);
            return VarFunctionCall(setter, args);
        }
    }
    // Use rawset
    return VarRawSet(table, key, value);
}

Var* ArgVarGet(Var* args, int index, char* key){
    DebugPrint("ArgVarGet\n");
    if(args->type != VAR_LIST){
        DebugPrint("ArgVarGet: args is not a list\n");
        return &NIL;
    }

    DebugPrint("ArgVarGet: args is a list\n");
    Var* v = VarRawGet(args, VarNewString(key));
    DebugPrint("ArgVarGet: v is %p\n", v);
    if ISUNDEFINED(v){
        DebugPrint("ArgVarGet: v is undefined\n");
        return VarRawGet(args, VarNewNumber(index));
    }
    return v;
}

// Attempts to ensure the output type is a string
Var* VarAsString(Var* var){
    DebugPrint("VarAsString\n");
    if(var->type == VAR_STRING){
        DebugPrint("VarAsString: already a string\n");
        return var;
    }

    DebugPrint("VarAsString: converting to string\n");
    Var* argList = VarNewList();
    VarRawSet(argList, VarNewString("obj"), var);
    VarRawSet(argList, VarNewNumber(0), var);
    DebugPrint("VarAsString: calling tostring\n");
    Var* result = VarFunctionCall(VarGetMeta(var, "tostring"), argList);
    if(result->type != VAR_STRING){
        DebugPrint("VarAsString: tostring did not return a string\n");
        return VarNewString("");
    }
    DebugPrint("VarAsString: done\n");
    return result;
}

#endif