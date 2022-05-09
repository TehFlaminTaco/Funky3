#ifndef GC_C
#define GC_C

#include "funky3.h"
#include "var.h"
#include "gc.h"
#include "hashmap.h"

void DerefferenceVar(Var* var, Var* from){
    if(var->referencedBy == NULL){
        // Root var, ignore.
        return;
    }
    // IF From is a list, check for any other references to this
    if(from->type == VAR_LIST){
        HashMap* map = (HashMap*)from->value;
        if(map -> parent == var){
            // We're still referneced here, so don't dereference
            return;
        }
        // Check the list's values for references to this var
        if(HashMapHasValue(map, var)){
            // We're still referenced here, so don't dereference
            return;
        }
    }
    LinkedListRemoveByValue(from->referencedBy, var);
    VarFreeLater(var);
    return;
}

int CheckValidity(Var* val, LinkedVarList* blacklist){
    // If we're at the root, return true.
    if(val->referencedBy == NULL){
        return 1;
    }
    // If we're in the blacklist, return false.
    for(int i = 0; i < LinkedListLength(blacklist); i++){
        if(LinkedListGet(blacklist, i) == val){
            return 0;
        }
    }
    // Append the value to the blacklist.
    LinkedListPush(blacklist, val);
    // If any value in referencedBy is valid, return true.
    for(int i = 0; i < LinkedListLength(val->referencedBy); i++){
        if(CheckValidity(LinkedListGet(val->referencedBy, i), blacklist)){
            return 1;
        }
    }
    // If we're here, we're not valid.
    return 0;
}

Var* VarFreeLater(Var* var){
    LinkedVarList* list = gc.toCleanup;
    LinkedListPush(list, var);
    return var;
}

void DoReferenceBy(Var* var, Var* from){
    if(var->referencedBy == NULL){
        return;
    }
    LinkedListRemoveByValue(var->referencedBy, from);
    LinkedListPush(var->referencedBy, from);
    return;
}

void GarbageCollect(){
    LinkedVarList* list = gc.toCleanup;
    for(int i=0; i<LinkedListLength(list); i++){
        Var* var = LinkedListGet(list, i);
        LinkedVarList* bl = LinkedListNew();
        if(!CheckValidity(var, bl)){
            FreeVar(var);
        }
        LinkedListFree(bl);
    }
    // Reset the GC List.
    LinkedListFree(list);
    gc.toCleanup = LinkedListNew();
}


#endif