#ifndef VAR_H
#define VAR_H

typedef struct _VAR {
    char type;
    long long value;
    struct _VAR* metatable;
} Var;

typedef struct _VARFUNCTION {
    Var* scope;
    Var* (*method)(Var*,Var*);
    char* name;
} VarFunction;

Var* VarNewNumber(double value);
Var* VarNewString(char* value);
Var* VarNewList();
Var* VarNewFunction(Var* (value)(Var*, Var*));

inline static Var* VarTrue();
inline static Var* VarFalse();

Var* VarFunctionCall(Var *function, Var *args);
Var* VarRawGet(Var* table, Var* key);
Var* VarRawSet(Var* table, Var* key, Var* value);
Var* ArgVarGet(Var* args, int index, char* key);
inline static Var* ArgVarSet(Var* args, int index, char* key, Var* value);

Var* VarGet(Var* table, Var* key);
Var* VarSet(Var* table, Var* key, Var* value);

int VarEquals(Var* var1, Var* var2);
Var* VarAsString(Var* var);
Var* VarAsCode(Var* var);
Var* VarAsFunction(Var* var);
int VarTruthy(Var* var);

Var* VarListCopy(Var* list);
Var* VarListCopyLShifted(Var* list, int shift);
Var* CallCurried(Var* scope, Var* args);
Var* VarCurryGet(Var* object, Var* index);
Var* MethodWithoutCurry(Var* scope, Var* args);
Var* VarCurrySet(Var* object, Var* index, Var* method);
Var* VarSubScope(Var* scope);
Var* VarWithScope(Var* scope, Var* with);

inline static int ISUNDEFINED(Var* var);

#endif