#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "linkedlist.h"
#include "var.h"
#include "var.c"
#include "linkedlist.c"
#include "gc.c"


GarbageCollector gc = {NULL};


Var UNDEFINED   = {0x00, 0, NULL, NULL, 0};
Var NIL         = {0x00, 1, NULL, NULL, 0};

int main(int argc, char** argv){
    gc.toCleanup = LinkedListNew();

    Var* StringA    = VarNew(0x02, (long long)"Henlo!", NULL);
    Var* StringB    = VarNew(0x02, (long long)"Henlo!", NULL);
    printf("??? %p", ((char*)StringA->value));
    if(VarEquals(StringA, StringB))
        printf("They are equal!\n");

    printf("Hello, World!");
    return 0;
}