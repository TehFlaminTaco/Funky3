# Math

The math library provides a number of functions for performing mathematical operations.\
Some methods, such as [`max`](#max), [`min`](#min) and [`clamp`](#clamp) are implemented such to allow for any type via the use of Metatables. This is done at the cost of speed.

## abs
**Description:**\
Return the absolute value of a number.

**Arguments:**
* number **number**: The number to get the absolute value of.

```coffeescript
math.abs(1)
```

## ceil
**Description:**\
Return the number rounded to positive infinity. An optional second argument can be provided to round to a specific radix.

**Arguments:**
* number **number**: The number to round.
* number? **radix**: The radix to round to.

```coffeescript
math.ceil(1.5) == 2
math.ceil(15, 10) == 20
```

## clamp
**Description:**\
Clamp a number between a minimum and maximum value. The arguments may be presented in any order, as this is calculated by determining the median of three values.

**Arguments:**
* number **number**: The number to clamp.
* number **min**: The minimum value.
* number **max**: The maximum value.

```coffeescript
math.clamp(1, 2, 3) == 2
math.clamp(1, 3, 2) == 2
math.clamp(3, 1, 2) == 2
```

## floor
**Description:**\
Return the number rounded to negative infinity. An optional second argument can be provided to round to a specific radix.

**Arguments:**
* number **number**: The number to round.
* number? **radix**: The radix to round to.

```coffeescript
math.floor(1.5) == 1
math.floor(15, 10) == 10
```

## max
**Description:**\
Return the largest of two or more values. Values are compared via the `lt` metamethod.

**Arguments:**
* number+ **values**: The values to compare.

```coffeescript
math.max(1, 2) == 2
math.max(1, 2, 3) == 3
math.max("Hello", "World") == "World"
```

## min
**Description:**\
Return the smallest of two or more values. Values are compared via the `gt` metamethod.

**Arguments:**
* number+ **values**: The values to compare.

```coffeescript
math.min(1, 2) == 1
math.min(1, 2, 3) == 1
math.min("Hello", "World") == "Hello"
```

## round
**Description:**\
Return the number rounded to the nearest integer. An optional second argument can be provided to round to a specific radix.

**Arguments:**
* number **number**: The number to round.
* number? **radix**: The radix to round to.

```coffeescript
math.round(1.5) == 2
math.round(1.4) == 1
math.round(15, 10) == 20
```