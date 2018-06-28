# Clip Geometry

This sample demonstrates how to clip a geometry with an envelope using the GeometryEngine.

![](screenshot.png)

Click or tap the "Clip" button to clip the blue graphic with the red envelopes.

## How it works
1. Create a `graphicsOverlay` containing an envelope.
2. Create another that contains three other envelopes: one that intersects a part of the first graphic, one that is entirely within it, and another completely outside of it.
3. Create one more that contains no graphics.
4. Create a button object that, when clicked, calls a function that hides the main graphic *only*.
5. In the same function, iterate through the overlay with the three envelopes. `forEach` one, a `Graphic` object is created using by calling `GeometryEngine.clip` method, passing in the main graphic and the current envelope in the iteration as arguments.
6. Add each iteration to the empty graphics overlay.

## Relevant API
 - GeometryEngine.clip


## Tags
Geometry, Clip, Envelopes, Graphics, GraphicsOverlay

