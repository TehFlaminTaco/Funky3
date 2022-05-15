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
        if(map -> withValue == var){
            // We're still referneced here, so don't dereference
            return;
        }
        if(!ISUNDEFINED(HashMapGetKey(map, var))){
            // We're still referneced here, so don't dereference
            return;
        }
        // Check the list's values for references to this var
        if(HashMapHasValue(map, var)){
            // We're still referenced here, so don't dereference
            return;
        }
    }
    LinkedListRemoveByValue(var->referencedBy, from);
    return;
}

int CheckValidity(Var* val, LinkedVarList* blacklist){
    DebugPrint("CheckValidity:\n");
    // If we're at the root, return true.
    if(val->referencedBy == NULL){
        DebugPrint("CheckValidity: root\n");
        return 1;
    }
    for(int i = LinkedListLength(val -> referencedBy)-1; i >= 0; i--){
        DebugPrint("CheckValidity: checking %i\n", i);
        Var* from = LinkedListGet(val -> referencedBy, i);
        DebugPrint("CheckValidity: val is %p\n", val);
        DebugPrint("CheckValidity: from is %p\n", from);
        DerefferenceVar(val, from); // Try to unreference if we can.
    }
    DebugPrint("CheckValidity: done\n");
    // If we're in the blacklist, return false.
    for(int i = 0; i < LinkedListLength(blacklist); i++){
        if(LinkedListGet(blacklist, i) == val){
            DebugPrint("CheckValidity: in blacklist\n");
            return 0;
        }
    }
    DebugPrint("CheckValidity: not in blacklist\n");
    // Append the value to the blacklist.
    LinkedListPush(blacklist, val);
    // If any value in referencedBy is valid, return true.
    for(int i = 0; i < LinkedListLength(val->referencedBy); i++){
        if(CheckValidity(LinkedListGet(val->referencedBy, i), blacklist)){
            DebugPrint("CheckValidity: referencedBy valid\n");
            return 1;
        }
    }
    DebugPrint("CheckValidity: referencedBy invalid\n");
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
    DebugPrint("Removing any old references\n");
    LinkedListRemoveByValue(var->referencedBy, from);
    DebugPrint("Adding new reference\n");
    LinkedListPush(var->referencedBy, from);
    return;
}

void GarbageCollect(){
    DebugPrint("GarbageCollect:\n");
    LinkedVarList* list = gc.toCleanup;
    for(int i=0; i<LinkedListLength(list); i++){
        DebugPrint("GarbageCollect: %i\n", i);
        Var* var = LinkedListGet(list, i);
        LinkedVarList* bl = LinkedListNew();
        if(!CheckValidity(var, bl)){
            DebugPrint("GarbageCollect: invalid (%p)\n", var);
            FreeVar(var);
        }
        LinkedListFree(bl);
    }
    // Reset the GC List.
    LinkedListFree(list);
    gc.toCleanup = LinkedListNew();
    DebugPrint("GarbageCollect: done\n");
}


#endif