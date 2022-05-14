# Globals

The following globals are available in Funky 3:

## pairs
**Description:**\
Returns an iterator that iterates over the key-value pairs of a table.

**Arguments:**\
* list **list**: The list to iterate over.

**Return Value:**\
function **iterator**: An iterator that returns the key-value pairs in the list.

```coffeescript
for (key, value) in pairs(list){
    print(key, value)
}
```


## print
**Description:**\
Writes a message to the console.

**Arguments:**
* any* **message**: The values to print.

**Return Value:**\
string **message**: The message that was printed.

**Special Notes:**\
The message will be printed with a newline appended.\
Arguements are separated by a tab.\
Arguments are implicitly converted to strings.

```coffeescript
print("Hello,", "World!")
```

## values
**Description:**\
Returns an iterator of all the values in a list.

**Arguments:**
* list **list**: The list to iterate over.

**Return Value:**\
function **iterator**: An iterator that returns the values in the list.

```coffeescript
for v in values(list) {
    print(v)
}
```
