# Meta
The Meta library provides functions and constants for accessing and handling metamethods and metatables.

## get
**Description:**\
Returns the metatable of a list.

**Arguments:**
* list **list**: The list to get the metatable of.

**Return Value:**\
list **metatable**: The metatable of the list.

```coffeescript
meta.get(l)
```

## set
**Description:**\
Sets the metatable of a list.

**Arguments:**
* list **list**: The list to set the metatable of.

**Return Value:**\
list **metatable**: The metatable that was set.

```coffeescript
meta.set(l, m)
```

# Constants
 * **base**
 * **function**
 * **number**
 * **string**
 * **list**
 * **null**

## methods
**Description:**\
A list of all metamethods. Keys are the metamethod names and values are functions which call the metamethod.

```coffeescript
meta.methods.add(3, 4)
```