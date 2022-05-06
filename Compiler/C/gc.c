#ifndef GC_C
#define GC_C

#include "var.c"

typedef struct {
    LinkedVarList* toCleanup;
} GarbageCollector;

void DerefferenceVar(Var* var, Var* from){
    LinkedVarList* list = var->referencedBy;
    Linklett* current = list->first;
    for(int i=0; i<LinkedListLength(list); i++){
        if(LinkedListGet(list, i) == from){
            LinkedListRemove(list, i);
            break;
        }
    }
    
}


#endif