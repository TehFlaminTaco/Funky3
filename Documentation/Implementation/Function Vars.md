# Var Function
Function variables store two values to allow for calling them later. They take the form of:

```c
typedef struct {
    Var* scope;
    void* method;
    char* name;
} VarFunction;
```

The name is included to allow stringification of a function.

The functions themselves always have the following signature:

```c
Var* Method01234567(Var* scope, Var* arguments){}
```

At the top of which, a sub-scope is generated with the arguments added.