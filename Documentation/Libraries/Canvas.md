# Canvas

he `draw.canvas` library provides functions for drawing to the specialized canvas. It is based on the [HTML5 Canvas API](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API).

## arc
**Description:**\
Draws an arc.

**Arguments:**
* number **x**: The x position of the arc.
* number **y**: The y position of the arc.
* number **radius**: The radius of the arc.
* number? **startAngle**: The angle to start the arc at.
* number? **endAngle**: The angle to end the arc at.
* boolean? **anticlockwise**: Whether to draw the arc in an anticlockwise direction.

```coffeescript
draw.canvas.arc(100, 100, 50)
```

## arcTo
**Description:**\
Draws an arc between two points.

**Arguments:**
* number **x1**: The x position of the first point.
* number **y1**: The y position of the first point.
* number **x2**: The x position of the second point.
* number **y2**: The y position of the second point.
* number **radius**: The radius of the arc.

```coffeescript
draw.canvas.arcTo(100, 100, 200, 200, 50)
```

## beginPath
**Description:**\
Starts a new path.

```coffeescript
draw.canvas.beginPath()
```

## bezierCurveTo
**Description:**\
Draws a bezier curve.

**Arguments:**
* number **cp1x**: The x position of the first control point.
* number **cp1y**: The y position of the first control point.
* number **cp2x**: The x position of the second control point.
* number **cp2y**: The y position of the second control point.
* number **x**: The x position of the end point.
* number **y**: The y position of the end point.

```coffeescript
draw.canvas.bezierCurveTo(100, 100, 200, 200, 300, 300)
```

## clear
**Description:**\
Clears the canvas.

```coffeescript
draw.canvas.clear()
```

## clip
**Description:**\
Clips the canvas to the current path.

```coffeescript
draw.canvas.clip()
```

## closePath
**Description:**\
Closes the current path.

```coffeescript
draw.canvas.closePath()
```

## drawImage
**Description:**\
Draws an image.

**Arguments:**
* string **image**: The image to draw.
* number **x**: The x position to draw the image at.
* number **y**: The y position to draw the image at.
* number? **width**: The width to draw the image at.
* number? **height**: The height to draw the image at.

```coffeescript
draw.canvas.drawImage("image", 100, 100)
```

## drawImage2
**Description:**\
Draws a portion of an image.

**Arguments:**
* string **image**: The image to draw.
* number **sx**: The x position of the portion of the image to draw.
* number **sy**: The y position of the portion of the image to draw.
* number **swidth**: The width of the portion of the image to draw.
* number **sheight**: The height of the portion of the image to draw.
* number **x**: The x position to draw the image at.
* number **y**: The y position to draw the image at.
* number **width**: The width to draw the image at.
* number **height**: The height to draw the image at.

```coffeescript
draw.canvas.drawImage2("image", 0, 0, 100, 100, 100, 100, 100, 100)
```

## fill
**Description:**\
Fills the current path.

```coffeescript
draw.canvas.fill()
```

## fillRect
**Description:**\
Fills a rectangle.

**Arguments:**
* number **x**: The x position of the rectangle.
* number **y**: The y position of the rectangle.
* number **width**: The width of the rectangle.
* number **height**: The height of the rectangle.

```coffeescript
draw.canvas.fillRect(100, 100, 100, 100)
```

## fillStyle
**Description:**\
Sets the fill style.

**Arguments:**
* string **style**: The style to set.

```coffeescript
draw.canvas.fillStyle("red")
```

## fillText
**Description:**\
Fills text.

**Arguments:**
* string **text**: The text to fill.
* number **x**: The x position of the text.
* number **y**: The y position of the text.

```coffeescript
draw.canvas.fillText("hello", 100, 100)
```

## font
**Description:**\
Sets the font.

**Arguments:**
* string **font**: The font to set.

```coffeescript
draw.canvas.font("30px Arial")
```

## isPointInPath
**Description:**\
Checks if a point is in the current path.

**Arguments:**
* number **x**: The x position of the point.
* number **y**: The y position of the point.

```coffeescript
draw.canvas.isPointInPath(100, 100)
```

## isPointInStroke
**Description:**\
Checks if a point is in the current stroke.

**Arguments:**
* number **x**: The x position of the point.
* number **y**: The y position of the point.

```coffeescript
draw.canvas.isPointInStroke(100, 100)
```

## lineTo
**Description:**\
Draws a line to a point.

**Arguments:**
* number **x**: The x position of the point.
* number **y**: The y position of the point.

```coffeescript
draw.canvas.lineTo(100, 100)
```

## lineWidth
**Description:**\
Sets the line width.

**Arguments:**
* number **width**: The width to set.

```coffeescript
draw.canvas.lineWidth(5)
```

## measureText
**Description:**\
Measures the size of text.

**Arguments:**
* string **text**: The text to measure.

```coffeescript
draw.canvas.measureText("hello")
```

## moveTo
**Description:**\
Moves the current path to a point.

**Arguments:**
* number **x**: The x position of the point.
* number **y**: The y position of the point.

```coffeescript
draw.canvas.moveTo(100, 100)
```

## quadraticCurveTo
**Description:**\
Draws a quadratic curve.

**Arguments:**
* number **cpx**: The x position of the control point.
* number **cpy**: The y position of the control point.
* number **x**: The x position of the end point.
* number **y**: The y position of the end point.

```coffeescript
draw.canvas.quadraticCurveTo(100, 100, 100, 100)
```

## rect
**Description:**\
Draws a rectangle.

**Arguments:**
* number **x**: The x position of the rectangle.
* number **y**: The y position of the rectangle.
* number **width**: The width of the rectangle.
* number **height**: The height of the rectangle.

```coffeescript
draw.canvas.rect(100, 100, 100, 100)
```

## reset
**Description:**\
Resets the canvas.

```coffeescript
draw.canvas.reset()
```

## resetTransform
**Description:**\
Resets the transform.

```coffeescript
draw.canvas.resetTransform()
```

## restore
**Description:**\
Restores the previous canvas state.

```coffeescript
draw.canvas.restore()
```

## rotate
**Description:**\
Rotates the canvas.

**Arguments:**
* number **angle**: The angle to rotate by.

```coffeescript
draw.canvas.rotate(5)
```

## roundRect
**Description:**\
Draws a rounded rectangle.

**Arguments:**
* number **x**: The x position of the rectangle.
* number **y**: The y position of the rectangle.
* number **width**: The width of the rectangle.
* number **height**: The height of the rectangle.
* number **radius**: The radius of the corners.

```coffeescript
draw.canvas.roundRect(100, 100, 100, 100, 10)
```

## save
**Description:**\
Saves the current canvas state.

```coffeescript
draw.canvas.save()
```

## scale
**Description:**\
Scales the canvas.

**Arguments:**
* number **x**: The x scale.
* number **y**: The y scale.

```coffeescript
draw.canvas.scale(2, 2)
```

## setTransform
**Description:**\
Sets the transform.

**Arguments:**
* number **a**: The a value.
* number **b**: The b value.
* number **c**: The c value.
* number **d**: The d value.
* number **e**: The e value.
* number **f**: The f value.

```coffeescript
draw.canvas.setTransform(1, 0, 0, 1, 0, 0)
```

## stroke
**Description:**\
Strokes the current path.

```coffeescript
draw.canvas.stroke()
```

## strokeRect
**Description:**\
Strokes a rectangle.

**Arguments:**
* number **x**: The x position of the rectangle.
* number **y**: The y position of the rectangle.
* number **width**: The width of the rectangle.
* number **height**: The height of the rectangle.

```coffeescript
draw.canvas.strokeRect(100, 100, 100, 100)
```

## strokeStyle
**Description:**\
Sets the stroke style.

**Arguments:**
* string **style**: The style to set.

```coffeescript
draw.canvas.strokeStyle("red")
```

## strokeText
**Description:**\
Strokes text.

**Arguments:**
* string **text**: The text to stroke.
* number **x**: The x position of the text.
* number **y**: The y position of the text.

```coffeescript
draw.canvas.strokeText("hello", 100, 100)
```

## textAlign
**Description:**\
Sets the text align.

**Arguments:**
* string **align**: The align to set.

```coffeescript
draw.canvas.textAlign("center")
```

## textBaseline
**Description:**\
Sets the text baseline.

**Arguments:**
* string **baseline**: The baseline to set.

```coffeescript
draw.canvas.textBaseline("center")
```

## transform
**Description:**\
Transforms the canvas.

**Arguments:**
* number **a**: The a value.
* number **b**: The b value.
* number **c**: The c value.
* number **d**: The d value.
* number **e**: The e value.
* number **f**: The f value.

```coffeescript
draw.canvas.transform(1, 0, 0, 1, 0, 0)
```

## translate
**Description:**\
Translates the canvas.

**Arguments:**
* number **x**: The x position to translate to.
* number **y**: The y position to translate to.

```coffeescript
draw.canvas.translate(100, 100)
```