#ifndef VAR_H
#define VAR_H

#define ISUNDEFINED(var) (var == NULL || var->type == VAR_NULL && var->value == 0)

typedef struct _VAR {
    char type;
    long long value;
    struct _VAR* metatable;
    struct _LinkedVarList* referencedBy; 
    char gcMark;
} Var;

typedef struct _VARFUNCTION {
    Var* scope;
    Var* (*method)(Var*,Var*);
    char* name;
} VarFunction;

Var* VarRawGet(Var* table, Var* key);
Var* VarRawSet(Var* table, Var* key, Var* value);
Var* ArgVarGet(Var* args, int index, char* key);

Var* VarGet(Var* table, Var* key);
Var* VarSet(Var* table, Var* key, Var* value);

int VarEquals(Var* var1, Var* var2);
Var* VarAsString(Var* var);
int VarTruthy(Var* var);

#endif