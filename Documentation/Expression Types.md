# Expression Types

## Assignment

**Description:**\
Assigns a value to a variable.

**Forms:**
* Expression: `Variable`, Expession: `Operator`, Punctuation: `=`, Expression: `Any`
* Expression: `Variable`, Punctuation: `=`, Expression: `Any`

## Block

**Description:**\
Many expressions that are all executed, and then the final value is returned.

**Forms:**
* Punctuation: `{`, Expression: `Any`*, Punctuation: `}`?

**Special Notes:**\
The final bracket being excluded does not constitute a syntax error.

## Crementor

**Description:**\
"Crementor", either an incrementor or Decrementor, is an expression that increments or decrements a variable.

**Forms:**
* Expression: `Variable`, Punctuation: `++`
* Expression: `Variable`, Punctuation: `--`
* Punctuation: `++`, Expression: `Variable`
* Punctuation: `--`, Expression: `Variable`

**Special Notes:**\
When the operator precedes the variable, the new value is returned. Otherwise, the old value is returned.

## Variable

**Description:**\
Allows one to access a stored value, typically from a scope.

**Forms:**
* Keyword: `var|local`, Identifier
* Identifier

**Special Notes:**\
When preceeded by `var` or `local`, the variable is declared as a local variable, and refer to the Top Most scope.