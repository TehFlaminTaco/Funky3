#ifndef GC_H
#define GC_H

#include "linkedlist.h"

typedef struct {
    LinkedVarList* toCleanup;
} GarbageCollector;

void DerefferenceVar(Var* var, Var* from);
Var* VarFreeLater(Var* var);
void GarbageCollect();
void DoReferenceBy(Var* var, Var* from);

#endif