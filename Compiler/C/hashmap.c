#ifndef HASHMAP_C
#define HASHMAP_C

#include "hashmap.h"

HashMap* HashMapNew(int capacity){
    HashMap* map = calloc(1, sizeof(HashMap));
    map->capacity = capacity;
    map->values = calloc(capacity, sizeof(LinkedKVList*));
    for(int i = 0; i < capacity; i++){
        map->values[i] = LinkedKVListNew();
    }
    return map;
}

void CalculateHash(Var* val){
    switch(val->type){
        case 0: // NIL
            return 0;
        case 1: // Double. Doubles are chaotic enough that this is an okay hash.
            return (int)val->value;
        case 2: // String
            int hash = 5381;
            int c;
            char* s = val->value;
            while(c = *s++){
                hash = ((hash << 5) + hash) + c;
            }
            return hash;

        // For pointers, the pointer is a good hash.
        case 3: // Function
            return (int)val->value;
        case 4: // List
            return (int)val->value;
        default:
            return 0;
}

#endif