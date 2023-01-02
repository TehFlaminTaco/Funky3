
#ifndef META_C
#define META_C

#include "lib.h"

#include "../funky3.h"
#include "../metatable.h"


Var* MetaGetMeta(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "obj");
    return v -> metatable;
}

Var* MetaSetMeta(Var* scope, Var* args){
    Var* v = ArgVarGet(args, 0, "obj");
    Var* m = ArgVarGet(args, 1, "metatable");
    v -> metatable = m;
    return v;
}

Var* MetaDoMethod(Var* scope, Var* args){
    Var* methodName = VarRawGet(scope, VarNewString("method"));
    if(methodName -> type != VAR_STRING){
        return &NIL;
    }
    char* methodNameStr = (char*) methodName -> value;
    Var* obj = ArgVarGet(args, 0, "obj");
    Var* method = VarGetMeta(obj, methodNameStr);
    return VarFunctionCall(method, args);
}

Var* _AddMetaMethod(Var* methods, char* name){
    Var* methodName = VarNewString(name);
    Var* methodScope = VarNewList();
    VarRawSet(methodScope, VarNewString("method"), methodName);
    return VarNewFunctionWithScope(MetaDoMethod, methodScope, "<C Function>");
}

void PopulateMetaLib(Var* meta){
    CONSTANT(get, VarNewFunction(MetaGetMeta));
    ALIAS(get, g);
    CONSTANT(set, VarNewFunction(MetaSetMeta));
    ALIAS(set, s);

    CONSTANT(base, &MetatableBase);
    ALIAS(base, b);
    CONSTANT(function, &MetatableFunction);
    ALIAS(function, f);
    CONSTANT(number, &MetatableNumber);
    ALIAS(number, n);
    CONSTANT(string, &MetatableString);
    ALIAS(string, s);
    CONSTANT(list, &MetatableList);
    ALIAS(list, l);
    CONSTANT(null, &MetatableNull);
    ALIAS(null, n);

    Var* methods = VarNewList();
    CONSTANT(methods, methods);
    _AddMetaMethod(methods, "add");
    _AddMetaMethod(methods, "sub");
    _AddMetaMethod(methods, "mul");
    _AddMetaMethod(methods, "div");
    _AddMetaMethod(methods, "intdiv");
    _AddMetaMethod(methods, "mod");
    _AddMetaMethod(methods, "pow");
    _AddMetaMethod(methods, "band");
    _AddMetaMethod(methods, "bor");
    _AddMetaMethod(methods, "bnot");
    _AddMetaMethod(methods, "bshl");
    _AddMetaMethod(methods, "bshr");
    _AddMetaMethod(methods, "lt");
    _AddMetaMethod(methods, "gt");
    _AddMetaMethod(methods, "le");
    _AddMetaMethod(methods, "ge");
    _AddMetaMethod(methods, "eq");
    _AddMetaMethod(methods, "ne");
    _AddMetaMethod(methods, "concat");
    _AddMetaMethod(methods, "unp");
    _AddMetaMethod(methods, "unm");
    _AddMetaMethod(methods, "not");
    _AddMetaMethod(methods, "bnot");
    _AddMetaMethod(methods, "len");
    _AddMetaMethod(methods, "iter");
    _AddMetaMethod(methods, "tostring");
    _AddMetaMethod(methods, "truthy");
    _AddMetaMethod(methods, "get");
    _AddMetaMethod(methods, "set");
    _AddMetaMethod(methods, "call");
    ALIAS(methods, m);
}

#endif