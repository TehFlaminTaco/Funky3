#ifndef HASHMAP_C
#define HASHMAP_C

#include "funky3.h"
#include "var.h"
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

int CalculateHash(Var* val){
    switch(val->type){
        case VAR_NULL: // NIL
            return 0;
        case VAR_NUMBER: // double. doubles are chaotic enough that this is an okay hash.
            return (int)val->value;
        case VAR_STRING: // String
            int hash = 5381;
            int c;
            char* s = (char*)val->value;
            while((c = *s++)){
                hash = ((hash << 5) + hash) + c;
            }
            return hash;

        // For pointers, the pointer is a good hash.
        case VAR_FUNCTION: // Function
            return (int)val->value;
        case VAR_LIST: // List
            return (int)val->value;
        default:
            return 0;
    }
}

void HashMapSet(HashMap* map, Var* key, Var* value){
    int hash = CalculateHash(key);
    if(hash < 0)hash = -hash;
    LinkedKVListInsert(map->values[hash % map->capacity], key, value);
    return;
}

Var* HashMapGet(HashMap* map, Var* key){
    DebugPrint("HashMapGet\n");
    int hash = CalculateHash(key);
    if(hash < 0)hash = -hash;
    DebugPrint("HashMapGet: hash is %d\n", hash);
    return LinkedKVListGet(map->values[hash % map->capacity], key);
}

Var* HashMapRemove(HashMap* map, Var* key){
    int hash = CalculateHash(key);
    if(hash < 0)hash = -hash;
    return LinkedKVListRemove(map->values[hash % map->capacity], key);
}

#endif