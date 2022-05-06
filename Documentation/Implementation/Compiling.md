# Compiling

Funky 3 implements a Tokenization step followed by a Comprehension step, then finally it is compiled to assembly, which is then assembled into a binary.

Currently, Funky 3 uses C as an intermediate language, but the goal is to target ASM.

## Tokenization

Funky 3 firstly splits the input into tokens. These take the form of:

* **Comment**: A comment that starts with a `$` and ends with a newline. May also start with `$*` and end with `*$`.
* **Keyword**: A keyword, such as `var` or `if`.
* **Identifier**: A word that starts with a letter and may contain numbers and underscores.
* **Number**: A number that may contain a decimal point.
* **String**: A string that starts and ends with quotes.
* **InterpolatedString**: A string that starts and ends with backticks and may contain interpolations, surrounded by square brackets. EG: `` `1+2 = [1+2]` ``
* **Punctuation**: A punctuation character. This includes brackets, commas, semicolons, etc.

## Comprehension

After tokenization, the tokens are then iterated over to combine them into expressions. Examples of expressions are:

| Name | Code Example | Tokenization | Description |
| ---- | ------------ | ------------ | ----------- |
| Assignment | `var x = 1` | `Keyword: "var", Identifier: "x", Punctuation: "=", Number: "1"` | Assigns a value to a variable. |
| IfStatement | `if x > 0 {}` | `Keyword: "if", Expression: "x > 0", Expression: "{}` | Executes a block of code if a condition is true. |

It is to be noted that Expressions may contain Expressions as a part of their definition, and are recursively defined. One type of expression may have many formats.