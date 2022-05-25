# List

The List library provides methods for working with lists.\
By default, these methods can be accessed from the `list` global, or as part of the default getter for list objects. EG:
```coffeescript
list.push(l, 1);
l:push(1);
```

Whilst lists are stored internally as HashMaps, some methods operate on the "array" portion of the list as seperate from the entire list.\
The array portion of the list are the values whos keys are a whole number starting at 0, and are less than the first empty key. Eg. `[1, 2, 3, undefined, 4]` will have the array portion of the list be `[1, 2, 3]`.\
Methods that operate only on the array portion will be marked with a `*` in the documentation.

A Concat function is not provided as lists may be joined via the `[a..., b...]` construct.

## clone
**Description:**\
Generate a new list with all the key/value pairs of the old list.

**Arguments:**
* list **list**: The list to clone.
* boolean **deep**: If true, the values will be cloned as well.

```coffeescript
list.clone(l)
list.clone(l, true)
```

## cumulate*
**Description:**\
Cumulates the values in a list.\
The cumulated list is the result of calling the function on the total of the previous values with the next value.\
`[1,2,3]` called with `@+` will result in `[1,3,6]`.

**Arguments:**
* list **list**: The list to cumulate.
* function(a, b) **func**: The function to apply to the cumulated values.
* any? **initial**: The initial value to start the cumulation with. It will not be in the final list.

```coffeescript
list.cumulate(l, @+)
list.cumulate(l, @+, 1)
```

## fold*
**Description:**\
Fold a list using a function.\
The folded list is the result of applying the function to each pair of elements on the list, and their results are stored as a new list. Use [reduce](#reduce) to reduce an entire list to a single value.

**Arguments:**
* list **list**: The list to fold.
* function(a, b) **function**: The function to apply to each pair of elements.
* any? **initial**: The initial value to start the fold with. If not provided, the first element of the list will be used.

``coffeescript
list.fold(l, @+)
list.fold(l, @+, 1)
```

## insert*, push*, enqueue*
**Description:**\
Insert an element into a list. \
If no index is provided, the element will be inserted at the start of the list.

**Arguments:**
* list **list**: The list to insert into.
* number? **index**: The index to insert the element at. If not provided, the element will be inserted at the start of the list.
* any **element**: The element to insert.

```coffeescript
list.insert(l, 1, 2);
list.push(l, 1, 2);
```

## map
**Description:**\
Returns a new list with the results of applying the given function to each element in the original list.

**Arguments:**
* list **list**: The list to map.
* function(value, key, list) **fn**: The function to apply to each element in the list.

```coffeescript
list.map(l, v => v + 1)
```

## min*, max*
**Description:**\
Returns the minimum or maximum value in a list.\
Optionally takes a function to select the value to compare.

**Arguments:**
* list **list**: The list to find the minimum or maximum value in.
* function(value, key, list)? **fn**: The function to select the value to compare.

```coffeescript
list.min(l)
list.min(l, v => v.length)
```

## reduce*
**Description:**\
Reduces a list to a single value.

**Arguments:**
* list **list**: The list to reduce.
* function(a, b) **fn**: The function to apply to each pair of elements.
* any? **initial**: The initial value to start the fold with. If not provided, the first element of the list will be used.

```coffeescript
list.reduce(l, (a, b) => a + b)
```

## remove*, pop*, dequeue*
**Description:**\
Remove an element from a list.\
If no index is provided, the first element of the list will be removed.\
`dequeue` is a special variant which instead removes the last element if no index is provided.

**Arguments:**
* list **list**: The list to remove from.
* number? **index**: The index of the element to remove. If not provided, the first element of the list will be removed.

```coffeescript
list.remove(l, 1);
list.pop(l);
```

## product*
**Description:**\
Calculate the product of all the elements in a list.\
Optionally takes a function to select the value to multiply.

**Arguments:**
* list **list**: The list to calculate the product of.
* function(value, key, list)? **fn**: The function to select the value to multiply.

```coffeescript
list.product(l)
list.product(l, v => v.length)
```

## reverse*
**Description:**\
Returns a new list with the elements in the original list in reverse order.

**Arguments:**
* list **list**: The list to reverse.

```coffeescript
list.reverse(l)
```

## sort*
**Description:**\
Returns a new list with the elements in the original list sorted.

**Arguments:**
* list **list**: The list to sort.
* function(a, b) **fn**: The comparison function. Truthy values will be sorted before falsy values.

```coffeescript
list.sort(l, @<);
```

## sum*
**Description:**\
Returns the sum of all the elements in the list.\
If a function is provided, the sum of the results of applying the function to each element in the list will be returned.

**Arguments:**
* list **list**: The list to sum.
* function(value, key, list)? **fn**: The function to apply to each element in the list.

```coffeescript
list.sum(l)
list.sum(l, v => v.length)
```

## where
**Description:**\
Returns a list of all the elements in a list that match a predicate.\
Elements that exist contiguously are considered to be array-style, and thus when they do not match the predicate will shift the other elements higher than it.\
Other elements, such as those seperated by an undefined, or keyed with non-integers, will be considered to be hash-style, and thus, will not shift the other elements and instead simply be removed.

**Arguments:**
* list **list**: The list to search.
* function(value, key, list) **predicate**: The predicate to match.

```coffeescript
list.where(l, v => v.length > 5)
```
