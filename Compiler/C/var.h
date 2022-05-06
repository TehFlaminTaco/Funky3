#ifndef VAR_H
#define VAR_H

typedef struct _VAR {
    char type;
    long long value;
    struct _VAR* metatable;
    struct _LinkedVarList* referencedBy; 
    char gcMark;
} Var;

#endif