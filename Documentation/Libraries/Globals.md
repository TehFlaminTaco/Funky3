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
Writes a message to the console. May optionally take in the `sep` and `end` named arguments which define a seperater and end of line text respectively.

**Arguments:**
* any* **message**: The values to print.
* string **sep**: The seperator to use between values.
* string **end**: The end of line text to use.

**Return Value:**\
string **message**: The message that was printed.

**Special Notes:**\
By default, the seperator is a tab, and the end of line text is a newline.

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

## write
**Description:**\
Writes a message to the console.

**Arguments:**\
* any* **message**: The values to print.
* string **sep**: The seperator to use between values.
* string **end**: The end of line text to use.

**Return Value:**\
string **message**: The message that was printed.

**Special Notes:**\
Identical to [`print`](#print), except that it uses a space as the default seperated, and has no default end of line.

```coffeescript
write("Hello,", "World!")
```