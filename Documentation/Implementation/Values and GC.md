# Value and GC

## Values

Values have the following structure

```C
struct Var{
    char type; // The builtin type. One of "Nil", "Number", "String", "Function", "Object"
    long value; // The value stored. May be a pointer.
    Var* metatable; // The metatable, defines behaviours of values.
    
    // These are used by the Garbage Collector
    LinkedList<Var*>* referencedBy; 
    char gcMark;
}
```

## Garbage Collection

When a value is dereferrenced, it's added to the GC queue. When the GC runs, it will free all values that are not referenced by anything.

### Simple Case
When a value has no references left, it is freed as nothing can access it.
### Self-reference protection
To protect against self-reference, we iterate through every value in referencedBy and check it's "Validity". A reference is Valid if it has a path to the execution root.

```C
/**
 * Checks if a val is still referenced
 * @param val The value to check
 * @param blacklist The list of values to ignore
 * @return True if the value is still referenced, false otherwise
 */
bool CheckValidity(Var* val, LinkedList<Var*>* blacklist){
    // If we're at the root, return true.
    if(val->referencedBy == NULL){
        return true;
    }
    // If we're in the blacklist, return false.
    for(int i = 0; i < LinkedListLength(blacklist); i++){
        if(LinkedListGet(blacklist, i) == val){
            return false;
        }
    }
    // Append the value to the blacklist.
    LinkedListAdd(blacklist, val);
    // If any value in referencedBy is valid, return true.
    for(int i = 0; i < LinkedListLength(val->referencedBy); i++){
        if(CheckValidity(LinkedListGet(val->referencedBy, i), blacklist)){
            return true;
        }
    }
    // If we're here, we're not valid.
    return false;
}
```