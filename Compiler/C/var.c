
#ifndef VAR_C
#define VAR_C

#include "var.h"

#include "linkedlist.c"

Var* VarNew(char type, long value, Var* metatable){
    Var* var = calloc(1, sizeof(Var));
    var->type = type;
    var->value = value;
    var->metatable = metatable;
    var->referencedBy = LinkedListNew();
    var->gcMark = 0;
    return var;
}

int VarEquals(Var* var1, Var* var2){
    if(var1->type != var2->type){
        return 0;
    }

    switch(var1->type){
        case 0: // NIL
            return var1->value == var2->value;
        case 1: // Double.
            return (double)var1->value == (double)var2->value;
        case 2: // String
            return strcmp((char*)(intptr_t)var1->value, (char*)(intptr_t)var2->value) == 0;
            //return (char*)(intptr_t)var2->value == NULL;
        

        // Function, Object
        case 3: case 4:
            return var1->value == var2->value;
        
        default:
            return 0;
    }
}

#endif