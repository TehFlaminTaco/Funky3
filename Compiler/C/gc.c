#ifndef GC_C
#define GC_C

#include "funky3.h"
#include "var.h"
#include "gc.h"
#include "hashmap.h"

void DerefferenceVar(Var* var, Var* from){
    LinkedVarList* list = var->referencedBy;
    if(list == NULL){
        // Root var, ignore.
        return;
    }
    for(int i=0; i<LinkedListLength(list); i++){
        if(LinkedListGet(list, i) == from){
            LinkedListRemove(list, i);
            break;
        }
    }
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