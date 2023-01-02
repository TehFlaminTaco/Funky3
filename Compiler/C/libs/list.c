#ifndef LIST_C
#define LIST_C

#include "lib.h"
#include <math.h>

#include "../funky3.h"
#include "../metatable.h"
#include "../hashmap.h"

Var* ListWhere(Var* scope, Var* args){
    Var* list = ArgVarGet(args, 0, "this");
    Var* func = VarAsFunction(ArgVarGet(args, 1, "func"));
    if(list -> type != VAR_LIST){
        return &NIL;
    }
    if(func -> type != VAR_FUNCTION){
        return &NIL;
    }

    // Identify the Max Index.
    int max = 0;
    while(!ISUNDEFINED(VarRawGet(list, VarNewNumber(max)))){
        max++;
    }

    Var* newList = VarNewList();
    HashMap* map = (HashMap*) list -> value;
    HashMap* newMap = (HashMap*) newList -> value;
    // Copy the Metatable.
    newList -> metatable = list -> metatable;

    // Iterate over each element in the old list
    int newIndex = 0;
    for(int i = 0; i < map -> capacity; i++){
        KVLinklett* current = map->values[i]->first;
        while(current != NULL){
            Var* key = current->key;
            Var* val = current->var;
            // Run the predicate on the key
            Var* args = VarNewList();
            ArgVarSet(args, 0, "value", val);
            ArgVarSet(args, 1, "key", key); // Incase you want to use these...
            ArgVarSet(args, 2, "list", list);
            Var* result = VarFunctionCall(func, args);
            if(!VarTruthy(result)){
                // Skip this element.
                current = current->next;
                continue;
            }
            if(key -> type == VAR_NUMBER){
                double j;
                memcpy(&j, &key -> value, sizeof(double));
                if(fmod(j, 1) > 0.000001 || j < 0 || j >= max){ // Non-integer or out of bounds.
                    VarRawSet(newList, key, current->var);
                }else{
                    VarRawSet(newList, VarNewNumber(newIndex++), current->var);
                }
            }else{
                VarRawSet(newList, key, current->var);
            }
            current = current->next;
        }
    }
    return newList;
}

Var* ListMap(Var* scope, Var* args){
    Var* list = ArgVarGet(args, 0, "this");
    Var* func = VarAsFunction(ArgVarGet(args, 1, "func"));
    if(list -> type != VAR_LIST){
        return &NIL;
    }
    if(func -> type != VAR_FUNCTION){
        return &NIL;
    }
    Var* newList = VarNewList();
    HashMap* map = (HashMap*) list -> value;
    HashMap* newMap = (HashMap*) newList -> value;
    // Copy the Metatable.
    newList -> metatable = list -> metatable;

    // Iterate over each element in the old list
    for(int i = 0; i < map -> capacity; i++){
        KVLinklett* current = map->values[i]->first;
        while(current != NULL){
            Var* key = current->key;
            Var* val = current->var;
            // Run the predicate on the key
            Var* args = VarNewList();
            ArgVarSet(args, 0, "value", val);
            ArgVarSet(args, 1, "key", key); // Incase you want to use these...
            ArgVarSet(args, 2, "list", list);
            VarRawSet(newList, key, VarFunctionCall(func, args));
            current = current->next;
        }
    }
    return newList;
}

Var* ListFold(Var* scope, Var* args){
    Var* list = ArgVarGet(args, 0, "this");
    Var* func = VarAsFunction(ArgVarGet(args, 1, "func"));
    Var* initial = ArgVarGet(args, 2, "initial");
    if(list -> type != VAR_LIST){
        return &NIL;
    }
    if(func -> type != VAR_FUNCTION){
        return &NIL;
    }
    Var* newList = VarNewList();
    HashMap* map = (HashMap*) list -> value;
    HashMap* newMap = (HashMap*) newList -> value;
    // Copy the Metatable.
    newList -> metatable = list -> metatable;

    // Iterate over each element in the old list
    int i = 0;
    int newIndex = 0;
    Var* key;
    Var* value = VarRawGet(list, key = VarNewNumber(i++));
    if(ISUNDEFINED(initial)){ // No first argument, skip and use first instead.
        initial = value;
        value = VarRawGet(list, key = VarNewNumber(i++));
    }
    while(!ISUNDEFINED(value)){
        Var* args = VarNewList();
        ArgVarSet(args, 0, "left", initial);
        ArgVarSet(args, 1, "right", value);
        VarRawSet(newList, VarNewNumber(newIndex++), VarFunctionCall(func, args));
        initial = value;
        value = VarRawGet(list, key = VarNewNumber(i++));
    }
    return newList;
}

Var* ListCumulate(Var* scope, Var* args){
    Var* list = ArgVarGet(args, 0, "this");
    Var* func = VarAsFunction(ArgVarGet(args, 1, "func"));
    Var* initial = ArgVarGet(args, 2, "initial");
    if(list -> type != VAR_LIST){
        return &NIL;
    }
    if(func -> type != VAR_FUNCTION){
        return &NIL;
    }
    Var* newList = VarNewList();
    HashMap* map = (HashMap*) list -> value;
    HashMap* newMap = (HashMap*) newList -> value;
    // Copy the Metatable.
    newList -> metatable = list -> metatable;

    // Iterate over each element in the old list
    int i = 0;
    int newIndex = 0;
    Var* key;
    Var* value = VarRawGet(list, key = VarNewNumber(i++));
    if(ISUNDEFINED(initial)){ // No first argument, skip and use first instead.
        initial = value;
        value = VarRawGet(list, key = VarNewNumber(i++));
        VarRawSet(newList, VarNewNumber(newIndex++), initial);
    }
    while(!ISUNDEFINED(value)){
        Var* args = VarNewList();
        ArgVarSet(args, 0, "left", initial);
        ArgVarSet(args, 1, "right", value);
        initial = VarFunctionCall(func, args);
        VarRawSet(newList, VarNewNumber(newIndex++), initial);
        value = VarRawGet(list, key = VarNewNumber(i++));
    }
    return newList;
}

Var* ListReduce(Var* scope, Var* args){
    Var* list = ArgVarGet(args, 0, "this");
    Var* func = VarAsFunction(ArgVarGet(args, 1, "func"));
    Var* initial = ArgVarGet(args, 2, "initial");
    if(list -> type != VAR_LIST){
        return &NIL;
    }
    if(func -> type != VAR_FUNCTION){
        return &NIL;
    }
    Var* newList = VarNewList();
    HashMap* map = (HashMap*) list -> value;
    HashMap* newMap = (HashMap*) newList -> value;
    // Copy the Metatable.
    newList -> metatable = list -> metatable;

    // Iterate over each element in the old list
    int i = 0;
    Var* key;
    Var* value = VarRawGet(list, key = VarNewNumber(i++));
    if(ISUNDEFINED(initial)){ // No first argument, skip and use first instead.
        initial = value;
        value = VarRawGet(list, key = VarNewNumber(i++));
    }
    while(!ISUNDEFINED(value)){
        Var* args = VarNewList();
        ArgVarSet(args, 0, "left", initial);
        ArgVarSet(args, 1, "right", value);
        initial = VarFunctionCall(func, args);
        value = VarRawGet(list, key = VarNewNumber(i++));
    }
    return initial;
}

Var* ListReverse(Var* scope, Var* args){
    Var* list = ArgVarGet(args, 0, "this");
    if(list -> type != VAR_LIST){
        return &NIL;
    }

    // Identify the Max Index.
    int max = 0;
    while(!ISUNDEFINED(VarRawGet(list, VarNewNumber(max)))){
        max++;
    }

    Var* newList = VarNewList();
    HashMap* map = (HashMap*) list -> value;
    HashMap* newMap = (HashMap*) newList -> value;
    // Copy the Metatable.
    newList -> metatable = list -> metatable;

    // Iterate over each element in the old list
    for(int i=0; i < max; i++){
        VarRawSet(newList, VarNewNumber(i), VarRawGet(list, VarNewNumber(max - i - 1)));
    }
    return newList;
}

Var* ListInsert(Var* scope, Var* args){
    // Insert an element into a list at a specific index.
    // Assumes an index of 0 if no index is given.
    Var* list = ArgVarGet(args, 0, "this");
    Var* index = ArgVarGet(args, 1, "index");
    Var* value = ArgVarGet(args, 2, "value");
    if(ISUNDEFINED(value)){
        value = index;
        index = VarNewNumber(0);
    }
    if(list -> type != VAR_LIST){
        return &NIL;
    }
    if(index -> type != VAR_NUMBER){
        return &NIL;
    }
    int max = 0;
    while(!ISUNDEFINED(VarRawGet(list, VarNewNumber(max)))){
        max++;
    }
    double j;
    memcpy(&j, &index -> value, sizeof(double));
    if(fmod(j, 1) > 0.00001){
        return &NIL;
    }

    int k = (int)j;
    if(k < 0) k = 0;
    if(k >= max){
        VarRawSet(list, VarNewNumber(max), value);
        return list;
    }
    int i = max;
    Var* nv = VarRawGet(list, VarNewNumber(--i));
    while(!ISUNDEFINED(nv)){
        if(i == k){
            VarRawSet(list, VarNewNumber(i), value);
            VarRawSet(list, VarNewNumber(i+1), nv);
        }else{
            VarRawSet(list, VarNewNumber(i+1), nv);
        }
        nv = VarRawGet(list, VarNewNumber(--i));
    }
    return list;
}

Var* ListRemove(Var* scope, Var* args){
    // Remove an element from a list at a specific index.
    // Assumes an index of 0 if no index is given.
    Var* list = ArgVarGet(args, 0, "this");
    Var* index = ArgVarGet(args, 1, "index");
    if(list -> type != VAR_LIST){
        return &NIL;
    }
    if(ISUNDEFINED(index)){
        index = VarNewNumber(0);
    }
    if(index -> type != VAR_NUMBER){
        Var* ret = VarRawGet(list, index);
        VarRawSet(list, index, &UNDEFINED);
        return ret;
    }
    double j;
    memcpy(&j, &index -> value, sizeof(double));
    if(fmod(j, 1) > 0.00001){
        Var* ret = VarRawGet(list, index);
        VarRawSet(list, index, &UNDEFINED);
        return ret;
    }

    int k = (int)j;
    if(k < 0){
        Var* ret = VarRawGet(list, index);
        VarRawSet(list, index, &UNDEFINED);
        return ret;
    }
    int i = 0;
    Var* retValue = &NIL;
    Var* v;
    while(!ISUNDEFINED(v=VarRawGet(list, VarNewNumber(i)))){
        if(i >= k){
            if(i==k){
                retValue = v;
            }
            VarRawSet(list, VarNewNumber(i), VarRawGet(list, VarNewNumber(i+1)));
        }
        i++;
    }
    if (i < k){ // If index was outside of array space, remove manually
        Var* ret = VarRawGet(list, index);
        VarRawSet(list, index, &UNDEFINED);
        return ret;
    }
    return retValue;
}

Var* ListDequeue(Var* scope, Var* args){
    // Remove an element from a list at a specific index.
    // Assumes the last element of the list if no index is given.
    Var* list = ArgVarGet(args, 0, "this");
    Var* index = ArgVarGet(args, 1, "index");
    if(list -> type != VAR_LIST){
        return &NIL;
    }
    if(ISUNDEFINED(index)){
        int max = 0;
        while(!ISUNDEFINED(VarRawGet(list, VarNewNumber(max)))){
            max++;
        }
        index = VarNewNumber(max - 1);
    }
    if(index -> type != VAR_NUMBER){
        Var* ret = VarRawGet(list, index);
        VarRawSet(list, index, &UNDEFINED);
        return ret;
    }
    double j;
    memcpy(&j, &index -> value, sizeof(double));
    if(fmod(j, 1) > 0.00001){
        Var* ret = VarRawGet(list, index);
        VarRawSet(list, index, &UNDEFINED);
        return ret;
    }

    int k = (int)j;
    if(k < 0){
        Var* ret = VarRawGet(list, index);
        VarRawSet(list, index, &UNDEFINED);
        return ret;
    }
    int i = 0;
    Var* retValue = &NIL;
    Var* v;
    while(!ISUNDEFINED(v=VarRawGet(list, VarNewNumber(i)))){
        if(i >= k){
            if(i==k){
                retValue = v;
            }
            VarRawSet(list, VarNewNumber(i), VarRawGet(list, VarNewNumber(i+1)));
        }
        i++;
    }
    if (i < k){ // If index was outside of array space, remove manually
        Var* ret = VarRawGet(list, index);
        VarRawSet(list, index, &UNDEFINED);
        return ret;
    }
    return retValue;
}

Var* ListClone(Var* scope, Var* args){
    // Clone a list.
    Var* list = ArgVarGet(args, 0, "this");
    int deep = VarTruthy(ArgVarGet(args, 1, "deep"));
    if(list -> type != VAR_LIST){
        return &NIL;
    }
    Var* newList = VarNewList();
    HashMap* map = (HashMap*) list -> value;
    HashMap* newMap = (HashMap*) newList -> value;
    // Copy the Metatable.
    newList -> metatable = list -> metatable;

    // Iterate over each element in the old list
    for(int i = 0; i < map -> capacity; i++){
        KVLinklett* current = map->values[i]->first;
        while(current != NULL){
            Var* key = current->key;
            Var* val = current->var;
            if(deep && !ISUNDEFINED(val) && val -> type == VAR_LIST){
                Var* newVal = VarNewList();
                Var* args = VarNewList();
                ArgVarSet(args, 0, "this", val);
                ArgVarSet(args, 1, "deep", VarTrue());
                VarRawSet(newList, key, ListClone(scope, args));
            }else{
                VarRawSet(newList, key, val);
            }
            current = current->next;
        }
    }
    return newList;
}

Var* cmpFunc;
int _listCmp(Var** A, Var** B){
    Var* a = *A;
    Var* b = *B;
    DebugPrint("ListSort: CmpFunc. %i\n", a->type);
    Var* args = VarNewList();
    ArgVarSet(args, 0, "left", a);
    ArgVarSet(args, 1, "right", b);
    if(VarTruthy(VarFunctionCall(cmpFunc, args))){
        return -1;
    }
    // Otherwise, Flip the Arguments. a < b becomes b < a.
    ArgVarSet(args, 0, "left", b);
    ArgVarSet(args, 1, "right", a);
    // If truthy, a > b, Otherwise, a == b
    return VarTruthy(VarFunctionCall(cmpFunc, args));
}

Var* ListSort(Var* scope, Var* args){
    DebugPrint("ListSort\n");
    Var* list = ArgVarGet(args, 0, "this");
    Var* func = VarAsFunction(ArgVarGet(args, 1, "func"));
    if(list -> type != VAR_LIST){
        return &NIL;
    }
    if(func -> type != VAR_FUNCTION){
        return &NIL;
    }
    int count = 0;
    while(!ISUNDEFINED(VarRawGet(list, VarNewNumber(count)))){
        count++;
    }
    // Store all the elements in an array to save operations.
    Var** elements = tgc_calloc(&gc, count, sizeof(Var*));
    for(int i = 0; i < count; i++){
        elements[i] = VarRawGet(list, VarNewNumber(i));
    }

    // Sort the array.
    Var* lastCmpFunc = cmpFunc;
    cmpFunc = func;
    DebugPrint("ListSort: cmpFunc set\n");
    qsort(elements, count, sizeof(Var*), _listCmp);
    DebugPrint("ListSort: qsort done\n");
    cmpFunc = lastCmpFunc;

    // Generate a new List
    Var* newList = VarNewList();
    DebugPrint("ListSort: newList created\n");
    for(int i = 0; i < count; i++){
        VarRawSet(newList, VarNewNumber(i), elements[i]);
    }
    DebugPrint("ListSort: newList populated\n");
    return newList;
}

Var* ListSum(Var* scope, Var* args){
    Var* list = ArgVarGet(args, 0, "this");
    Var* selector = VarAsFunction(ArgVarGet(args, 1, "func"));
    Var* initial = ArgVarGet(args, 2, "initial");
    if(list -> type != VAR_LIST){
        return &NIL;
    }
    Var* newList = VarNewList();
    HashMap* map = (HashMap*) list -> value;
    HashMap* newMap = (HashMap*) newList -> value;
    // Copy the Metatable.
    newList -> metatable = list -> metatable;

    // Iterate over each element in the old list
    int i = 0;
    Var* key;
    Var* value = VarRawGet(list, key = VarNewNumber(i++));
    if(ISUNDEFINED(initial)){ // No first argument, skip and use first instead.
        initial = value;
        value = VarRawGet(list, key = VarNewNumber(i++));
        if(selector -> type == VAR_FUNCTION){
            Var* cArgs = VarNewList();
            ArgVarSet(cArgs, 0, "value", initial);
            ArgVarSet(cArgs, 1, "index", VarNewNumber(0));
            ArgVarSet(cArgs, 2, "list", list);
            initial = VarFunctionCall(selector, cArgs);
        }
    }
    Var* func = VarGetMeta(initial, "add");
    while(!ISUNDEFINED(value)){
        if(selector -> type == VAR_FUNCTION){
            Var* cArgs = VarNewList();
            ArgVarSet(cArgs, 0, "value", value);
            ArgVarSet(cArgs, 1, "index", VarNewNumber(0));
            ArgVarSet(cArgs, 2, "list", list);
            value = VarFunctionCall(selector, cArgs);
        }
        Var* args = VarNewList();
        ArgVarSet(args, 0, "left", initial);
        ArgVarSet(args, 1, "right", value);
        initial = VarFunctionCall(func, args);
        value = VarRawGet(list, key = VarNewNumber(i++));
    }
    return initial;
}

Var* ListProduct(Var* scope, Var* args){
    Var* list = ArgVarGet(args, 0, "this");
    Var* initial = ArgVarGet(args, 1, "initial");
    if(list -> type != VAR_LIST){
        return &NIL;
    }
    Var* newList = VarNewList();
    HashMap* map = (HashMap*) list -> value;
    HashMap* newMap = (HashMap*) newList -> value;
    // Copy the Metatable.
    newList -> metatable = list -> metatable;

    // Iterate over each element in the old list
    int i = 0;
    Var* key;
    Var* value = VarRawGet(list, key = VarNewNumber(i++));
    if(ISUNDEFINED(initial)){ // No first argument, skip and use first instead.
        initial = value;
        value = VarRawGet(list, key = VarNewNumber(i++));
    }
    Var* func = VarGetMeta(initial, "mul");
    while(!ISUNDEFINED(value)){
        Var* args = VarNewList();
        ArgVarSet(args, 0, "left", initial);
        ArgVarSet(args, 1, "right", value);
        initial = VarFunctionCall(func, args);
        value = VarRawGet(list, key = VarNewNumber(i++));
    }
    return initial;
}

Var* ListMax(Var* scope, Var* args){
    Var* list = ArgVarGet(args, 0, "this");
    Var* selector = VarAsFunction(ArgVarGet(args, 1, "func"));
    if(list -> type != VAR_LIST){
        return &NIL;
    }
    Var* curr = VarRawGet(list, VarNewNumber(0));
    if(ISUNDEFINED(curr)){
        return &NIL;
    }
    if(!ISUNDEFINED(selector)){
        Var* cargs = VarNewList();
        ArgVarSet(cargs, 0, "value", curr);
        ArgVarSet(cargs, 1, "index", VarNewNumber(0));
        ArgVarSet(cargs, 2, "list", list);
        curr = VarFunctionCall(selector, cargs);
    }
    Var* right;
    for(int i = 1; !ISUNDEFINED(right=VarRawGet(list, VarNewNumber(i))); i++){
        Var* cargs = VarNewList();
        if(!ISUNDEFINED(selector)){
            ArgVarSet(cargs, 0, "value", right);
            ArgVarSet(cargs, 1, "index", VarNewNumber(i));
            ArgVarSet(cargs, 2, "list", list);
            right = VarFunctionCall(selector, cargs);
            cargs = VarNewList();
        }
        Var* lt = VarGetMeta(curr, "lt");
        if(lt -> type != VAR_FUNCTION){
            return &NIL;
        }
        ArgVarSet(cargs, 0, "left", curr);
        ArgVarSet(cargs, 1, "right", right);
        if(VarTruthy(VarFunctionCall(lt, cargs))){
            curr = right;
        }   
    }
    return curr;
}

Var* ListMin(Var* scope, Var* args){
    Var* list = ArgVarGet(args, 0, "this");
    Var* selector = VarAsFunction(ArgVarGet(args, 1, "func"));
    if(list -> type != VAR_LIST){
        return &NIL;
    }
    Var* curr = VarRawGet(list, VarNewNumber(0));
    if(ISUNDEFINED(curr)){
        return &NIL;
    }
    if(!ISUNDEFINED(selector)){
        Var* cargs = VarNewList();
        ArgVarSet(cargs, 0, "value", curr);
        ArgVarSet(cargs, 1, "index", VarNewNumber(0));
        ArgVarSet(cargs, 2, "list", list);
        curr = VarFunctionCall(selector, cargs);
    }
    Var* right;
    for(int i = 1; !ISUNDEFINED(right=VarRawGet(list, VarNewNumber(i))); i++){
        Var* cargs = VarNewList();
        if(!ISUNDEFINED(selector)){
            ArgVarSet(cargs, 0, "value", right);
            ArgVarSet(cargs, 1, "index", VarNewNumber(i));
            ArgVarSet(cargs, 2, "list", list);
            right = VarFunctionCall(selector, cargs);
            cargs = VarNewList();
        }
        Var* gt = VarGetMeta(curr, "gt");
        if(gt -> type != VAR_FUNCTION){
            return &NIL;
        }
        ArgVarSet(cargs, 0, "left", curr);
        ArgVarSet(cargs, 1, "right", right);
        if(VarTruthy(VarFunctionCall(gt, cargs))){
            curr = right;
        }   
    }
    return curr;
}


Var* ListRawGet(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "obj");
    Var* k = ArgVarGet(args, 1, "key");
    return VarRawGet(v, k);
}

Var* ListRawSet(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "obj");
    Var* k = ArgVarGet(args, 1, "key");
    Var* v2 = ArgVarGet(args, 2, "value");
    VarRawSet(v, k, v2);
    return v2;
}

Var* ListToList(Var* scope, Var* args){
    // Get the iterator of the object and make a list out of it.
    Var* v = ArgVarGet(args, 0, "obj");
    if(v -> type == VAR_LIST){
        return v;
    }
    Var* func;
    Var* nArgs = VarNewList();
    ArgVarSet(nArgs, 0, "obj", v);
    if(v->type == VAR_FUNCTION){
        func = v;
    }else{
        func = VarAsFunction(VarGetMeta(v, "iter"));
        if(func -> type != VAR_FUNCTION){ // Not iteratable.. :(
            return &NIL;
        }
        func = VarFunctionCall(func, nArgs);
    }
    if(func -> type != VAR_FUNCTION){ // Not iteratable.. :(
        return &NIL;
    }
    Var* outList = VarNewList();
    Var* res;
    int index = 0;
    while(!ISUNDEFINED(res = VarFunctionCall(func, nArgs))){
        VarRawSet(outList, VarNewNumber(index++), res);
        ArgVarSet(nArgs, 1, "last", res);
    }
    return outList;
}

void PopulateListLib(Var* list){
    // Set the default getter for List's to point to this library.
    VarRawSet(&MetatableList, VarNewString("get"), list);

    CONSTANT(where, VarNewFunction(ListWhere));
    CONSTANT(map, VarNewFunction(ListMap));
    CONSTANT(fold, VarNewFunction(ListFold));
    CONSTANT(reduce, VarNewFunction(ListReduce));
    CONSTANT(cumulate, VarNewFunction(ListCumulate));
    CONSTANT(reverse, VarNewFunction(ListReverse));
    CONSTANT(insert, VarNewFunction(ListInsert));
    ALIAS(insert, push);
    ALIAS(insert, enqueue);
    CONSTANT(remove, VarNewFunction(ListRemove));
    ALIAS(remove, pop);
    CONSTANT(dequeue, VarNewFunction(ListDequeue));
    CONSTANT(clone, VarNewFunction(ListClone));
    CONSTANT(sort, VarNewFunction(ListSort));

    CONSTANT(sum, VarNewFunction(ListSum));
    CONSTANT(product, VarNewFunction(ListProduct));
    CONSTANT(max, VarNewFunction(ListMax));
    CONSTANT(min, VarNewFunction(ListMin));

    CONSTANT(rawGet, VarNewFunction(ListRawGet));
    CONSTANT(rawSet, VarNewFunction(ListRawSet));

    CONSTANT(toList, VarNewFunction(ListToList));

    ALIAS(where, w);
    ALIAS(map, m);
    ALIAS(fold, f);
    ALIAS(reduce, r);
    ALIAS(cumulate, c);
    ALIAS(reverse, R);
    ALIAS(insert, p);
    ALIAS(remove, P);
    ALIAS(dequeue, d);
    ALIAS(clone, C);
    ALIAS(sort, S);

    ALIAS(sum, s);
    ALIAS(product, M);
    ALIAS(min, x);
    ALIAS(max, X);

    ALIAS(rawGet, rG);
    ALIAS(rawSet, rS);
    ALIAS(toList, tL);
}

#endif