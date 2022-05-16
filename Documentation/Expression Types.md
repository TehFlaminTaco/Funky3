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
Arguments may be named with the form `name = value`.

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

## DeOperator

**Description**\
The DeOperator is a unary operator that turns a statement or an operator into a function.

**Forms:**
* Punctuation: `@`, Punctuation: `Operator`
* Punctuation: `@`, Expression: `Any`

**Special Notes:**\
Operators are given precedence over statements. So `print(@!v)` is equivalent to `print(@!, v)`, rather than `print(@(!v))`. Use parentheses to override this.

## For

**Description:**\
A for loop.

**Forms:**
* Punctuation: `for`, Punctuation: `(`, Expression: `Any`, Expression: `Any`, Expression: `Any`, Punctuation: `)`, Expression: `Any`

**Special Notes:**\
The first expression is evaluated once, and then the second expression is checked for truthyness. The third expression is evaluated after each iteration. This is a C Style For loop.

## For In

**Description:**\
A for loop that iterates over an iterator. The iterator is returned by the `iter` metamethod, unless the passed value is a function.

**Forms:**
* Punctuation: `for`, Punctuation: `(`?, Expression: `Any`, Keyword: `in`, Expression: `Any`, Punctuation: `)`?, Expression: `Any`

**Special Notes:**\
An iterator is a function that, when called repeatedly, returns the next value in the sequence. Once every value is returned, the iterator should return `undefined`. It's important to note that `nil` will not terminate, and be looped over instead.

## Function

**Description:**\
Defines a function.

**Forms:**
* Keyword: `function`, Expression: `Variable`?, Punctuation: `(`, Expression: `Any`*, Punctuation: `)`, Expression: `Any`
* Expression: `Variable`, Punctuation: `=>`, Expression: `Any`
* Punctuation: `(`, Expression: `Variable`*, Punctuation: `)`, Punctuation: `=>`, Expression: `Any`

**Special Notes:**\
Arguments may be "splat"ed, any number of times, by appending a `...` to the end of the argument.
Arguments may be named with the form `name = value`.

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

## Variable Indexing

**Description:**\
Access the stored value within a list. Can also be used to generate Curry Methods.

**Forms:**
* Expression: `Any`, Punctuation: `[`, Expression: `Any`, Punctuation: `]`
* Expression: `Any`, Punctuation: `.`, Identifier: `Any`
* Expression: `Any`, Punctuation: `:`, Identifier: `Any`

**Special Notes:**\
The first form allows one to index with strange objects, like other lists, nil, or numbers.\
The second form only allows one to index with a string.\
The third form will always resolve to a special function which passes itself as the first argument automatically.

## While

**Description:**\
A while loop. Whilst the first expression is truthy, the second expression is evaluated.

**Forms:**
* Keyword: `while`, Expression: `Any`, Expression: `Any`

## With

**Description:**\
Expose a list of variables to a scope.

**Forms:**
* Keyword: `with`, Expression: `Any`*, Punctuation: `{`, Expression: `Any`*, Punctuation: `}`
* Keyword: `with`, Expression: `Any`*, Expression: `Any`

**Special Notes:**\
The final bracket being excluded does not constitute a syntax error.\
A block is not treated as a seperate expression, and such local values are referrenced directly from the value.