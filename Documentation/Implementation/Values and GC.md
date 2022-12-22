# Value and GC

## Values

Values have the following structure

```C
struct Var{
    char type; // The builtin type. One of "Nil", "Number", "String", "Function", "Object"
    void* value; // The value stored. May be a pointer.
    Var* metatable; // The metatable, defines behaviours of values.
}
```

## Garbage Collection

Garbage collection is now handled by [tgc](https://github.com/orangeduck/tgc) mixed with some hopes and prayers.