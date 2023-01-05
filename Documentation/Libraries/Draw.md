# Draw
The `draw` library provides methods for rendering in 2D space.

## box
**Description:**\
Draws a rectangle

**Arguments:**
* number **x**: The left x position of the box
* number **y**: The top y position of the box
* number **w**: The width of the box
* number **h**: The height of the box

```coffeescript
draw.setColor(255,0,0,255);
draw.box(10, 10, 10, 10);
```

## boxOutline
**Description:**\
Draws the outline of a rectangle

**Arguments:**
* number **x**: The left x position of the box
* number **y**: The top y position of the box
* number **w**: The width of the box
* number **h**: The height of the box

```coffeescript
draw.setColor(255,0,0,255);
draw.boxOutline(10, 10, 10, 10);
```

## cameraToScreen
**Description:**\
Converts worldspace coordinates to screen coordinates via a specified camera.

**Arguments:**
* number **x**: The x position in the world
* number **y**: The y position in the world
* list **position**: The position of the camera. As a list containing an `x` and `y` value.
* list?|number? **size**: The size of the camera view. As a number, specifies the larger of the width/height. As a list, contains one or both of `w` and `h`, and optionally an `aspectRatio` named value.
* number? **angle**: The rotation of the view.

**Return Value:**\
list **position**: Returns a list containing an `x` and `y` value.

```coffeescript
draw.cameraToScreen(15, 15, [15, 15], [10, 10]); $$ [x=256, y=256]
using var cam = draw.pushCamera([15, 15], [10, 10]) {
    draw.cameraToScreen(15, 15, cam...); $$ [x=256, y=256]
}
```

## circle
**Description:**\
Draws the a circle

**Arguments:**
* number **x**: The center x position of the circle
* number **y**: The center y position of the circle
* number **r**: The radius

```coffeescript
draw.circle(10,10,8);
```

## circleOutline
**Description:**\
Draws the outline of a circle

**Arguments:**
* number **x**: The center x position of the circle
* number **y**: The center y position of the circle
* number **r**: The radius

```coffeescript
draw.circleOutline(10,10,8);
```

## clear
**Description:**\
Clears the screen

```coffeescript
draw.clear();
```

## getColor
**Description:**\
Returns the last set color.

**Return Value:**\
list **rgba**: The color stored as a list. Keyed by both name and number in the order of r, g, b, a.

```coffeescript
draw.getColor();
```

## getFont
**Description:**\
returns the last set font.

```coffeescript
draw.getFont();
```

## getLineWidth
**Description:**\
Gets the current line width.

```coffeescript
draw.getLineWidth();
```

## pop
**Description:**\
Pops the transform stack.

```coffeescript
draw.pop();
```

## print
**Description:**\
Draws text to the screen.

**Arguments:**
* string **text**: The text to draw
* number **x**: The x position of the text
* number **y**: The y position of the text
* number? **ox**: The x anchor between 0.0 and 1.0
* number? **oy**: The y anchor between 0.0 and 1.0

**Return Value:**\
string **text**: The text input.

```coffeescript
draw.print("Hello, World!", 10, 10);
```

## printOutline
**Description:**\
Draws the outline of text to the screen.

**Arguments:**
* string **text**: The text to draw
* number? **x**: The x position of the text
* number? **y**: The y position of the text
* number? **ox**: The x anchor between 0.0 and 1.0
* number? **oy**: The y anchor between 0.0 and 1.0

**Return Value:**\
string **text**: The text input.

```coffeescript
draw.printOutline("Hello, World!", 10, 10);
```

## push
**Description:**\
Pushs to the transform stack, allows you to `translate`, `rotate`, or `scale`, then return to the current transform.

**Return Value:**\
list **disposable**: A Disposable. When disposed, executes `pop`.

```coffeescript
using (draw.push()){

}
```

## pushCamera
**Description:**
Pushs to the transform stack and transforms it so that elements are drawn within the view of a specified camera.

**Arguments:**
* list **position**: The position of the camera. As a list containing an `x` and `y` value.
* list?|number? **size**: The size of the camera view. As a number, specifies the larger of the width/height. As a list, contains one or both of `w` and `h`, and optionally an `aspectRatio` named value.
* number? **angle**: The rotation of the view.

**Return Value:**\
list **camera**: The camera. Contains both the inputs, and is disposable. When disposed, executes `pop`.

```coffeescript
using (var cam = draw.pushCamera(position = [75, 75], size = [50, 50])){

}
```

## pushViewport
**Description:**
Pushs to the transform stack and transforms it so that elements drawn within the viewport are positioned on screen.

**Arguments:**
* number **left**: The left limit of the viewport
* number **top**: The top limit of the viewport
* number **right**: The right limit of the viewport
* number **bottom**: The bottom limit of the viewport

**Return Value:**\
list **viewport**: The viewport. Contains both the limits, and is disposable. When disposed, executes `pop`.

```coffeescript
using (var vp = draw.pushViewport(50, 50, 100, 100)){

}
```

## rotate
**Description:**\
Rotates the current transform

**Arguments:**
* number **angle**: The angle in radians to rotate by.

```coffeescript
using draw.push() {
    draw.rotate(math.pi / 4);
}
```

## scale
**Description:**\
Scales the current transform

**Arguments:**
* number **x**: The amount to scale the x axis by
* number? **y**: The amount to scale the y axis by. Otherwise, the x value.

```coffeescript
using draw.push() {
    draw.scale(2, 2);
}
```

## screenHeight
**Description:**\
Provides the height of the current screen.

**Return Value:**\
number **height**: The height of the current screen

```
draw.screenHeight()
```

## screenSize
**Description:**\
Provides the size of the current screen as a list.

**Return Value:**\
list **size**: A list containing an `x` and `y` value as the width and height.

```
draw.screenSize()
```

## screenToCamera
**Description:**\
Converts screen coordinates to worldspace coordinates via a specified camera.

**Arguments:**
* number **x**: The x position on screen
* number **y**: The y position on screen
* list **position**: The position of the camera. As a list containing an `x` and `y` value.
* list?|number? **size**: The size of the camera view. As a number, specifies the larger of the width/height. As a list, contains one or both of `w` and `h`, and optionally an `aspectRatio` named value.
* number? **angle**: The rotation of the view.

**Return Value:**\
list **position**: A list containing an `x` and `y` value.

```coffeescript
draw.screenToCamera(256, 256, [15, 15], [10, 10]); $$ [x=15, y=15]
using var vp = draw.pushCamera([15, 15], [10, 10]) {
    draw.screenToCamera(256, 256, vp...); $$ [x=15, y=15]
}
```

## screenToViewport
**Description:**\
Converts screen coordinates to worldspace coordinates via a specified viewport.

**Arguments:**
* number **x**: The x position on screen
* number **y**: The y position on screen
* number **left**: The left limit of the viewport
* number **top**: The top limit of the viewport
* number **right**: The right limit of the viewport
* number **bottom**: The bottom limit of the viewport

**Return Value:**\
list **position**: A list containing an `x` and `y` value.

```coffeescript
draw.screenToViewport(256, 256, 10, 10, 20, 20); $$ [x=15, y=15]
using var vp = draw.pushViewport(10, 10, 20, 20) {
    draw.screenToViewport(256, 256, vp...); $$ [x=15, y=15]
}
```

## screenWidth
**Description:**\
Provides the width of the current screen.

**Return Value:**\
number **width**: The width of the current screen

```
draw.screenWidth()
```

## setColor
**Description:**\
Sets the colour for future draw operations.
All values are between 0 and 255.

**Arguments:**
* number **r**: The Red value
* number **b**: The Blue value
* number **g**: The Green value
* number? **a**: The Alpha value. Otherwise, 255.

**Return Value:**\
* list **color**: The input color. As per `getColor`. Additionally, this list can be disposed to restore the last color.

```coffeescript
draw.setColor(255,0,0,255);
```

## drawSetFont
**Description:**\
Sets teh font for future text drawing operations.

**Arguments:**
* string **font**: The name of the new font.

**Return Value:**\
list **disposable**: A disposable which, when disposed, restores the last font.

```coffeescript
draw.setFont("52px Comic Sans MS");
```

## setLineWidth
**Description:**\
Sets the line width of all line based draw operations, in world-space.

**Arguments:**
* number **width**: The width
list **disposable**: A disposable which, when disposed, restores the last line width.

**Return Value:**\


```coffeescript
draw.setLineWidth(1);
```

## translate
**Description:**\
Translates the current transform

**Arguments:**
* number **x**: The amount to translate the x axis by
* number? **y**: The amount to translate the y axis by. Otherwise, the x value.

```coffeescript
using draw.push() {
    draw.translate(-256, -256);
}
```

## viewportToScreen
**Description:**\
Converts worldspace coordinates to screen coordinates via a specified viewport.

**Arguments:**
* number **x**: The x position in the world
* number **y**: The y position in the world
* number **left**: The left limit of the viewport
* number **top**: The top limit of the viewport
* number **right**: The right limit of the viewport
* number **bottom**: The bottom limit of the viewport

**Return Value:**\
list **position**: Returns a list containing an `x` and `y` value.

```coffeescript
draw.viewportToScreen(15, 15, 10, 10, 20, 20); $$ [x=256, y=256]
using var vp = draw.pushViewport(10, 10, 20, 20) {
    draw.viewportToScreen(15, 15, vp...); $$ [x=256, y=256]
}
```