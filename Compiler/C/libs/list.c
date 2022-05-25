#ifndef LIST_C
#define LIST_C

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
    HashMap* map = list -> value;
    HashMap* newMap = newList -> value;
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
    HashMap* map = list -> value;
    HashMap* newMap = newList -> value;
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
    HashMap* map = list -> value;
    HashMap* newMap = newList -> value;
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
    HashMap* map = list -> value;
    HashMap* newMap = newList -> value;
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
    HashMap* map = list -> value;
    HashMap* newMap = newList -> value;
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
    HashMap* map = list -> value;
    HashMap* newMap = newList -> value;
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
    HashMap* map = list -> value;
    HashMap* newMap = newList -> value;
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

void PopulateListLib(Var* list){
    // Set the default getter for List's to point to this library.
    VarRawSet(&MetatableList, VarNewString("get"), list);

    VarRawSet(list, VarNewString("where"), VarNewFunction(ListWhere));
    VarRawSet(list, VarNewString("map"), VarNewFunction(ListMap));
    VarRawSet(list, VarNewString("fold"), VarNewFunction(ListFold));
    VarRawSet(list, VarNewString("reduce"), VarNewFunction(ListReduce));
    VarRawSet(list, VarNewString("cumulate"), VarNewFunction(ListCumulate));
    VarRawSet(list, VarNewString("reverse"), VarNewFunction(ListReverse));
    VarRawSet(list, VarNewString("insert"), VarNewFunction(ListInsert));
    VarRawSet(list, VarNewString("push"), VarNewFunction(ListInsert)); // Push is an alias for insert.
    VarRawSet(list, VarNewString("enqueue"), VarNewFunction(ListInsert)); // Enqueue is an alias for insert.
    VarRawSet(list, VarNewString("remove"), VarNewFunction(ListRemove));
    VarRawSet(list, VarNewString("pop"), VarNewFunction(ListRemove)); // Pop is an alias for remove.
    VarRawSet(list, VarNewString("dequeue"), VarNewFunction(ListDequeue));
    VarRawSet(list, VarNewString("clone"), VarNewFunction(ListClone));
    VarRawSet(list, VarNewString("sort"), VarNewFunction(ListSort));

}

#endif