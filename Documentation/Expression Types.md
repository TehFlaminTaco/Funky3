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

## Call


**Description:**\
Calls a function.

**Forms:**
* Expression: `method`, Expression: `(`, Expression: `Any`*, Punctuation: `)`

**Special Notes:**\
The final bracket being excluded does not constitute a syntax error.
Arguments may optionally be seperated by commas.
Arguments may be "splat"ed, any number of times, by appending a `...` to the end of the argument.

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

## Function

**Description:**\
Defines a function.

**Forms:**
* Keyword: `function`, Expression: `Variable`?, Punctuation: `(`, Expression: `Any`*, Punctuation: `)`, Expression: `Any`
* Expression: `Variable`, Punctuation: `=>`, Expression: `Any`
* Punctuation: `(`, Expression: `Variable`*, Punctuation: `)`, Punctuation: `=>`, Expression: `Any`

## Math

**Description:**\
Performs math on one or two expressions with an operator.

**Forms:**
* Expression: `Expression`, Expression: `Operator, Binary`, Expression: `Expression`
* Expression: `Operator, Unary`, Expression: `Expression`

## Operator, Binary

**Description:**\
A binary operator is an operator that takes two operands. This expression in particular is meant to be used with a Math expression, or an assignment.

**Forms:**
* Punctuation: `+`
* Punctuation: `-`
* Punctuation: `*`
* Punctuation: `//`
* Punctuation: `/`
* Punctuation: `%`
* Punctuation: `^`
* Punctuation: `&`
* Punctuation: `|`
* Punctuation: `~`
* Punctuation: `<<`
* Punctuation: `>>`
* Punctuation: `<`
* Punctuation: `>`
* Punctuation: `<=`
* Punctuation: `>=`
* Punctuation: `==`
* Punctuation: `!=`
* Punctuation: `&&`
* Punctuation: `||`
* Keyword: `and`
* Keyword: `or`

**Special Notes:**\
`^` is the Expentiation operator.\
`~` is the Bitwise XOR operator.\
`and`, `or`, `&&`, and `||` are the logical operators. They allow for short-circuit evaluation.

## Operator, Unary

**Description:**\
A unary operator is an operator that takes one operand. This expression in particular is meant to be used with a Math Expression or Crementor expression.

**Forms:**
* Punctuation: `+`
* Punctuation: `-`
* Punctuation: `!`
* Punctuation: `~`
* Keyowrd: `not`

## Variable

**Description:**\
Allows one to access a stored value, typically from a scope.

**Forms:**
* Keyword: `var|local`, Identifier
* Identifier

**Special Notes:**\
When preceeded by `var` or `local`, the variable is declared as a local variable, and refer to the Top Most scope.

## With

**Description:**\
Expose a list of variables to a scope.

**Forms:**
* Keyword: `with`, Expression: `Any`*, Punctuation: `{`, Expression: `Any`*, Punctuation: `}`
* Keyword: `with`, Expression: `Any`*, Expression: `Any`

**Special Notes:**\
The final bracket being excluded does not constitute a syntax error.\
A block is not treated as a seperate expression, and such local values are referrenced directly from the value.