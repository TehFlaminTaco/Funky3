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

## deg
**Description:**\
Convert an angle from radians to degrees.

**Arguments:**
* number **radians**: The angle in radians.

```coffeescript
math.deg(math.pi) == 180
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

## rad
**Description:**\
Return the number of radians for the given number of degrees.

**Arguments:**
* number **degrees**: The number of degrees.

```coffeescript
math.rad(180) == math.pi
```

## random
**Description:**\
Return a random number.\
If no arguments are provided, the number will be between 0 and 1.\
If one argument is provided, the number will be between 0 and the argument.\
If two arguments are provided, the number will be between the first and second argument.\
The output is a floating point number. To make it an integer, try int-dividing the result by one.

**Arguments:**
* number? **min**: The minimum value.
* number? **max**: The maximum value.

```coffeescript
math.random() == [0, 1)
math.random(10) == [0, 10)
math.random(10, 20) == [10, 20)
```

## randomseed
**Description:**\
Set the seed for the random number generator.\
This is cast to an integer, so `1.2` and `1` will both set the seed to 1.

**Arguments:**
* number **seed**: The seed to use.

```coffeescript
math.randomseed(12)
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

## sqrt
**Description:**\
Return the square root of a number.

**Arguments:**
* number **number**: The number to get the square root of.

```coffeescript
math.sqrt(4) == 2
```

# Trigonometry

## acos
**Description:**\
Return the arc cosine of a number.

**Arguments:**
* number **number**: The number to get the arc cosine of.

```coffeescript
math.acos(0) == math.pi / 2
math.acos(1) == 0
math.acos(-1) == math.pi
```

## asin
**Description:**\
Return the arc sine of a number.

**Arguments:**
* number **number**: The number to get the arc sine of.

```coffeescript
math.asin(0) == 0
math.asin(1) == math.pi / 2
```

## atan
**Description:**\
Return the arc tangent of a number.\
May optionally accept a second argument to calculate the Arc Tangent of `y/x`.


**Arguments:**
* number **y**: The number to get the arc tangent of.
* number? **x**: The number to get the arc tangent of.

```coffeescript
math.atan(0) == 0
math.atan(1,1) == math.pi / 4
```

## cos
**Description:**\
Return the cosine of a number.

**Arguments:**
* number **number**: The number to get the cosine of.

```coffeescript
math.cos(0) == 1
math.cos(math.pi / 2) == 0
math.cos(math.pi * 2) == 1
```

## sin
**Description:**\
Return the sine of a number.

**Arguments:**
* number **number**: The number to get the sine of.

```coffeescript
math.sin(0) == 0
math.sin(math.pi / 2) == 1
math.sin(math.pi * 2) == 0
```

## tan
**Description:**\
Return the tangent of a number.

**Arguments:**
* number **number**: The number to get the tangent of.

```coffeescript
math.tan(0) == 0
```

# Constants
* **pi:** `3.141593`
* **e:** `2.718282`
* **huge:** `inf`<sup>1</sup>
* **inf:** `inf`
* **nan:** `nan`
* **epsilon:** `0.000001`

<sup>1</sup>Whilst `huge` and `inf` both stringify as `"inf"`, `huge` is not actually `inf`, and is just a number that is larger than any other number. Consequently, it is not considered equal to `inf`.