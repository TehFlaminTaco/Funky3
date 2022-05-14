# Metatables
Metatables define the behavior of a variable. There are default metatables associated with each type, and lists may have custom metatables.

## Metamethods

### Math Metamethods
The following metamethods are associated with the math operators:
* `+`: `add`
* `-`: `sub`
* `*`: `mul`
* `/`: `div`
* `//`: `intdiv`
* `%`: `mod`
* `^`: `pow`
* `&`: `band`
* `|`: `bor`
* `~`: `bnot`
* `<<`: `bshl`
* `>>`: `bshr`
* `<`: `lt`
* `>`: `gt`
* `<=`: `le`
* `>=`: `ge`
* `==`: `eq`
* `!=`: `ne`
* `..`: `concat`

### Iter
The `iter` metamethod is called when a list is iterated over. It should return an iterator function. See "For In" for how these should be structured.

### To String
`tostring` is a metamethod that is called when a variable is converted to a string. If this does not return a string, an empty string is returned.

### Truthy
`truthy` is a metamethod that is called when a variable is tested for truthiness. If this does not return a boolean, `false` is returned.

### Get
`get` is used to add custom indexing behavior to a variable. It takes the arguments: `table`, `key`.

### Set
`set` is used to add custom indexing behavior to a variable. It takes the arguments: `table`, `key`, `value`.
