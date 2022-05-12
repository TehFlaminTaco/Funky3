
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
        var->value = NULL;
    }
    if(var->metatable != NULL)
        DerefferenceVar(var->metatable, var);
    var->metatable = NULL;
    if(var->referencedBy != NULL)
        LinkedListFree(var->referencedBy);
    var->referencedBy = NULL;
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
    func->name = "<C Function>";
    return VarFreeLater(VarNew(VAR_FUNCTION, (long long)func, &MetatableFunction));
}

Var* VarNewFunctionWithScope(Var* (value)(Var*, Var*), Var* scope, char* name){
    VarFunction* func = calloc(1, sizeof(VarFunction));
    Var* vFunc = VarNew(VAR_FUNCTION, (long long)func, &MetatableFunction);
    func->method = value;
    func->scope = VarSubScope(scope);
    DoReferenceBy(func->scope, vFunc);
    func->name = name;
    return VarFreeLater(vFunc);
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
    function = VarAsFunction(function);
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
    if(key -> type == VAR_STRING){
        DebugPrint("VarRawGet: key is \"%s\"\n", (char*)(intptr_t)key->value);
    }
    if(table->type != VAR_LIST){
        DebugPrint("VarRawGet: table is not a list\n");
        return &UNDEFINED;
    }

    DebugPrint("VarRawGet: table is a list\n");
    HashMap* map = (HashMap*)table->value;
    // WithMeta hack
    if(table->metatable == &MetatableWith){
        if(key->type == VAR_STRING && strcmp(key->value, "this") == 0){
            return map -> withValue;
        }
        return VarRawGet(map -> withValue, key);
    }
    DebugPrint("VarRawGet: map is %p\n", map);
    Var* result = HashMapGet(map, key);
    return result;
}

Var* VarRawSet(Var* table, Var* key, Var* value){
    DebugPrint("VarRawSet\n");
    if(key == NULL){
        DebugPrint("VarRawSet: key is NULL\n");
        return &UNDEFINED;
    }
    if(table->type != VAR_LIST){
        DebugPrint("VarRawSet: table is not a list\n");
        return &UNDEFINED;
    }
    HashMap* map = (HashMap*)table->value;
    // WithMeta hack
    if(table->metatable == &MetatableWith){
        return VarRawSet(map -> withValue, key, value);
    }

    // Remove any currently set var from the referencedby list.
    DebugPrint("VarRawSet: removing old value\n");
    Var* oldValue = VarRawGet(table, key);
    DebugPrint("VarRawSet: old value is %p\n", oldValue);
    if(oldValue != NULL && oldValue->referencedBy != NULL){
        DebugPrint("VarRawSet: removing old value from referencedby list\n");
        LinkedListRemoveByValue(oldValue->referencedBy, table);
    }
    // Add to the referencedby list
    DebugPrint("VarRawSet: adding new value to referencedby list\n");
    if(value->referencedBy != NULL){
        DebugPrint("VarRawSet: adding new value to referencedby list\n");
        DoReferenceBy(value, table);
    }

    // Remove any currently set key from the referencedby list.
    DebugPrint("VarRawSet: removing old key\n");
    Var* oldKey = HashMapGetKey(map, key);
    DebugPrint("VarRawSet: old key is %p\n", oldKey);
    if(!ISUNDEFINED(oldKey)){
        DebugPrint("VarRawSet: removing old key from referencedby list\n");
        if(oldKey -> referencedBy != NULL){
            LinkedListRemoveByValue(oldKey->referencedBy, table);
        }
    }

    // Adding to key's referencedby list
    DebugPrint("VarRawSet: adding new key to referencedby list\n");
    if(key->referencedBy != NULL){
        DebugPrint("VarRawSet: adding new key to referencedby list\n");
        DoReferenceBy(key, table);
    }

    if(ISUNDEFINED(value)){
        DebugPrint("VarRawSet: removing key\n");
        HashMapRemove(map, key);
    }else{
        DebugPrint("VarRawSet: setting value\n");
        HashMapSet(map, key, value);
    }
    DebugPrint("VarRawSet: done\n");
    return value;
}

Var* VarGet(Var* table, Var* key){
    DebugPrint("VarGet\n");
    // Try Rawget first
    Var* value = VarRawGet(table, key);
    if(!ISUNDEFINED(value)){
        return value;
    }
    // Use the "get" metamethod
    Var* getter = VarGetMeta(table, "get");
    if(getter->type == VAR_FUNCTION){
        // Call the getter
        Var* args = VarNewList();
        VarRawSet(args, VarNewString("table"), table);
        VarRawSet(args, VarNewString("key"), key);
        VarRawSet(args, VarNewNumber(0), table);
        VarRawSet(args, VarNewNumber(1), key);
        value = VarFunctionCall(getter, args);
        if(!ISUNDEFINED(value)){
            return value;
        }   
    }

    // Try and get from the Parent.
    if(table -> type == VAR_LIST){
        DebugPrint("VarGet: table is a list\n");
        HashMap* map = (HashMap*)table->value;
        if(map == NULL){
            DebugPrint("VarGet: map is NULL\n");
            return &UNDEFINED;
        }
        Var* parent = map -> parent;
        if(!ISUNDEFINED(parent)){
            DebugPrint("VarGet: parent is not undefined\n");
            return VarGet(parent, key);
        }
        DebugPrint("VarGet: parent is undefined\n");
    }

    return &UNDEFINED;
}

Var* VarSet(Var* table, Var* key, Var* value){
    DebugPrint("VarSet\n");
    // Is there anything defined?
    Var* oldValue = VarRawGet(table, key);
    if(ISUNDEFINED(oldValue)){
        DebugPrint("VarSet: nothing defined\n");
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
        return &UNDEFINED;
    }

    DebugPrint("ArgVarGet: args is a list\n");
    Var* v = &UNDEFINED;
    if(key != NULL)
        v = VarRawGet(args, VarNewString(key));
    DebugPrint("ArgVarGet: v is %p\n", v);
    if ISUNDEFINED(v){
        DebugPrint("ArgVarGet: v is undefined\n");
        return VarRawGet(args, VarNewNumber(index));
    }
    return v;
}

Var* ArgVarSet(Var* args, int index, char* key, Var* value){
    return VarRawSet(args, VarNewString(key), VarRawSet(args, VarNewNumber(index), value));
}

// Attempts to ensure the output type is a string
Var* VarAsString(Var* var){
    DebugPrint("VarAsString (%i)\n", var->type);
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

Var* VarAsCode(Var* var){
    DebugPrint("VarAsCode\n");
    Var* argList = VarNewList();
    VarRawSet(argList, VarNewString("obj"), var);
    VarRawSet(argList, VarNewNumber(0), var);
    DebugPrint("VarAsCode: calling tocode\n");
    Var* result = VarFunctionCall(VarGetMeta(var, "tocode"), argList);
    if(result->type != VAR_STRING){
        DebugPrint("VarAsCode: tocode did not return string\n");
        return VarNewString("");
    }
    DebugPrint("VarAsCode: done\n");
    return result;
}

Var* VarAsFunction(Var* var){
    DebugPrint("VarAsFunction\n");
    if(ISUNDEFINED(var)){
        DebugPrint("VarAsFunction: var is undefined\n");
        return &UNDEFINED;
    }
    if(var->type == VAR_FUNCTION){
        DebugPrint("VarAsFunction: already a function\n");
        return var;
    }
    // Return the call metamethod
    Var* method = VarGetMeta(var, "call");
    if(method->type != VAR_FUNCTION){
        DebugPrint("VarAsFunction: no call metamethod\n");
        return &UNDEFINED;
    }
    // Curry the Call function so that it passes itself as an argument.
    VarFunction* fMethod = method -> value;
    Var* tempScope = VarNewList();
    VarRawSet(tempScope, VarNewString("this"), var);
    VarRawSet(tempScope, VarNewString("method"), method);
    Var* fnc = VarNewFunction(CallCurried);
    VarFunction* f = fnc -> value;
    f -> scope = tempScope;
    f -> name = fMethod -> name;
    return fnc;
}

int VarTruthy(Var* var){
    // If a number is non-zero, it's truthy.
    if(var->type == VAR_NUMBER){
        double j;
        memcpy(&j, &var->value, sizeof(double));
        return fabs(j - 0) > 0.00001;
    }

    // Otherwise, check the metatable.
    Var* argList = VarNewList();
    VarRawSet(argList, VarNewString("obj"), var);
    VarRawSet(argList, VarNewNumber(0), var);
    Var* truthy = VarGetMeta(var, "truthy");
    if(truthy->type != VAR_FUNCTION){
        return 0;
    }
    Var* result = VarFunctionCall(truthy, argList);
    if(result->type != VAR_NUMBER){
        return 0;
    }
    double j;
    memcpy(&j, &result->value, sizeof(double));
    return fabs(j - 0) > 0.00001;
}

Var* VarListCopy(Var* list){
    DebugPrint("VarListCopy\n");
    if(list->type != VAR_LIST){
        DebugPrint("VarListCopy: not a list\n");
        return &NIL;
    }
    Var* newList = VarNewList();
    newList -> metatable = list -> metatable;
    HashMap* map = list -> value;
    HashMap* newMap = newList -> value;
    newMap -> parent = map -> parent;

    // Copy all values over
    for(int i=0; i < map -> capacity; i++){
        KVLinklett* current = map->values[i]->first;
        while(current != NULL){
            DoReferenceBy(current->key, newList);
            DoReferenceBy(current->var, newList);
            HashMapSet(newMap, current->key, current->var);
            current = current->next;
        }
    }

    return newList;
}

Var* VarListCopyLShifted(Var* list, int shift){
    DebugPrint("VarListCopy\n");
    if(list->type != VAR_LIST){
        DebugPrint("VarListCopy: not a list\n");
        return &NIL;
    }
    Var* newList = VarNewList();
    newList -> metatable = list -> metatable;
    HashMap* map = list -> value;
    HashMap* newMap = newList -> value;
    newMap -> parent = map -> parent;

    // Copy all values over
    DebugPrint("VarListCopy: copying values\n");
    for(int i=0; i < map -> capacity; i++){
        KVLinklett* current = map->values[i]->first;
        while(current != NULL){
            DebugPrint("VarListCopy: copying values: %i/%i, %p\n", i, map->capacity - 1, current);
            if(current->key -> type == VAR_NUMBER){
                // If it's an integer, shift it
                double j;
                memcpy(&j, &current->key->value, sizeof(double));
                if(fmod(j,1) < 0.00001){
                    j = j - shift;
                    if(j >= 0){
                        DoReferenceBy(current->key, newList);
                        DoReferenceBy(current->var, newList);
                        HashMapSet(newMap, VarNewNumber(j), current->var);
                    }
                }else{
                    DoReferenceBy(current->key, newList);
                    DoReferenceBy(current->var, newList);
                    HashMapSet(newMap, current->key, current->var);
                }
            }else{
                DoReferenceBy(current->key, newList);
                DoReferenceBy(current->var, newList);
                HashMapSet(newMap, current->key, current->var);
            }
            current = current->next;
        }
    }
    DebugPrint("VarListCopy: done\n");
    return newList;
}

/*
function curry(method, a){
    return b...=>{
        return method(a, b...);
    }
}
*/

Var* CallCurried(Var* scope, Var* args){
    Var* method = VarRawGet(scope, VarNewString("method"));
    Var* this = VarRawGet(scope, VarNewString("this"));
    Var* callArgs = VarListCopyLShifted(args, -1);
    VarRawSet(callArgs, VarNewNumber(0), this);
    return VarFunctionCall(method, callArgs);
}

Var* VarCurryGet(Var* object, Var* index){
    DebugPrint("VarCurryGet\n");
    Var* method = VarAsFunction(VarRawGet(object, index));
    if(method -> type != VAR_FUNCTION){
        DebugPrint("VarCurryGet: not a function\n");
        return &UNDEFINED;
    }
    VarFunction* fMethod = method -> value;
    Var* tempScope = VarNewList();
    VarRawSet(tempScope, VarNewString("this"), object);
    VarRawSet(tempScope, VarNewString("method"), method);
    Var* fnc = VarNewFunction(CallCurried);
    VarFunction* f = fnc -> value;
    f -> scope = tempScope;
    f -> name = fMethod -> name;
    return fnc;
}

Var* MethodWithoutCurry(Var* scope, Var* args){
    DebugPrint("MethodWithoutCurry\n");
    Var* method = VarRawGet(scope, VarNewString("method"));
    Var* this = ArgVarGet(args, 0, "this");
    DebugPrint("MethodWithoutCurry: Copying Args!\n");
    Var* callArgs = VarListCopyLShifted(args, 1);
    VarRawSet(callArgs, VarNewString("this"), this);
    DebugPrint("MethodWithoutCurry: Calling Method\n");
    return VarFunctionCall(method, callArgs);
}

Var* VarCurrySet(Var* object, Var* index, Var* method){
    DebugPrint("VarCurrySet\n");
    method = VarAsFunction(method);
    if(method -> type != VAR_FUNCTION){
        DebugPrint("VarCurrySet: not a function\n");
        return &UNDEFINED;
    }
    VarFunction* fMethod = method -> value;
    Var* tempScope = VarNewList();
    VarRawSet(tempScope, VarNewString("this"), object);
    VarRawSet(tempScope, VarNewString("method"), method);
    Var* fnc = VarNewFunction(MethodWithoutCurry);
    DebugPrint("VarCurrySet: created function\n");
    VarFunction* f = fnc -> value;
    f -> scope = tempScope;
    f -> name = fMethod -> name;
    DebugPrint("VarCurrySet: set scope\n");
    return VarRawSet(object, index, fnc);
}


// Generate a Scope using the previous scope as a parent.
Var* VarSubScope(Var* scope){
    DebugPrint("VarSubScope\n");
    if(scope->type != VAR_LIST){
        DebugPrint("VarSubScope: not a list\n");
        return &NIL;
    }
    Var* newScope = VarNewList();
    HashMap* map = newScope -> value;
    map -> parent = scope;
    DoReferenceBy(scope, newScope);
    
    return newScope;
}

// Entirely for usage with the with(e){} block
Var* VarWithScope(Var* scope, Var* with){
    DebugPrint("VarWithScope\n");
    if(scope->type != VAR_LIST){
        DebugPrint("VarWithScope: not a list\n");
        return &NIL;
    }
    Var* newScope = VarNewList();
    newScope -> metatable = &MetatableWith;
    HashMap* map = newScope -> value;
    map -> parent = scope;
    DoReferenceBy(scope, newScope);
    map -> withValue = with;
    DoReferenceBy(with, newScope);

    return newScope;
}

#endif