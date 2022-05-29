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

## rawGet
**Description:**\
Returns the value of a key in a list. Ignores parents and metamethods.

**Arguments:**
* list **list**: The list to get the value from.
* any **key**: The key to get the value of.

**Return Value:**\
any **value**: The value of the key.

```coffeescript
print(rawGet(list, "key"))
```

## rawSet
**Description:**\
Sets the value of a key in a list. Ignores parents and metamethods.

**Arguments:**
* list **list**: The list to set the value in.
* any **key**: The key to set the value of.
* any **value**: The value to set.

**Return Value:**\
any **value**: The value that was set.

```coffeescript
rawSet(list, "key", "value")
```

## toList
**Description:**\
Converts a value to a list via the Iterator Function. if this was already a list, returns as is.

**Arguments:**
* any **value**: The value to convert to a list.

**Return Value:**\
list **list**: The list that was created.

```coffeescript
print(toList(1))
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