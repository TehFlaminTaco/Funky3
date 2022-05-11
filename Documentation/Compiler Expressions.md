# Compiler Expressions

Compiler expressions are expressions that directly affect the compiled code, or that require compile-time execution.

A simple example is Macros, whilst a more complex example is directly inserting C code.

All Compiler Expressions start with a `$` followed by a keyword, and then arguments are parsed from the rest of the line.

## $cHeaderStart

    $cHeaderStart
    ...
    $cHeaderEnd

Like `$cInlineStart`, but the code is prepended to the header file. This is useful for including other C libraries, or defining C functions and types.

See Implementation for more details on how to use the language specific C.

## $cInlineStart

    $cInlineStart
    ...
    $cInlineEnd

Runs the code between the `$cInlineStart` and `$cInlineEnd` markers as pure C. This is useful when you want to communicate with unmanaged code, or just want to run some C code. Implicitely, whatever value is stored on `_` is returned.

See Implementation for more details on how to use the language specific C.

## $define

    $define name value
    $define/name(\w+)/value$1

Much like C, you can define macros. The macros may be simple, taking the form of `$define name value`, or more complex and allow for regular expressions, such as `$define/name(\w+)/value$1`.

Unlike C, the simple form cannot be used to define macros that take arguments. The more complex form may be used to define psudo-arguments, via regular expressions.