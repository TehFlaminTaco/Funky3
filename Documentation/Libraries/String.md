# String

Regular Expressions for the string library are implemented via [small-regex-c](https://gitlab.com/relkom/small-regex). As such, they only offer a limited subset of regular expressions.

## find
**Description:**\
Find the first match of a regular expression in a string and return it's index. If no match is found, return -1. An optional third "offset" argument can be provided to start searching from a specific index.

**Arguments:**
* string **haystack**: The string to search.
* string **needle**: The regular expression to search for.
* number? **offset**: The offset to start searching from.

```coffeescript
string.find(haystack, needle)
```

## match
**Description:**\
Match a string against a regular expression. If it matches, return the full match. If it doesn't match, return `nil`.\
An optional third "offset" argument can be provided to start the search at a given position.

**Arguments:**
* string **haystack**: The string to match against.
* string **needle**: The regular expression to match.
* number? **offset**: The offset to start the search at. If not provided, the search will start at the beginning of the string.

```coffeescript
string.match(haystack, "^[a-z]+$")
string.match(haystack, "^[a-z]+$", 7)
```

## sub
**Description:**\
Get a substring of a string.\
If no length is provided, the substring will be the rest of the string.\
If the first argument is negative, and no length is provided, the substring will be the rest of the string from the end.\
If the length is negative, the substring will match untill this many characters from the end.\
The string is assumed to be an infinite repeating copy of itself in both directions.

**Arguments:**
* string **string**: The string to get a substring of.
* number **start**: The index to start the substring from.
* number? **length**: The length of the substring.

```coffeescript
string.sub(string, 2)
string.sub(string, 2, 5)
string.sub(string, -2)
```