# Clip Geometry

This sample demonstrates how to clip a geometry with an envelope using the GeometryEngine.

![](screenshot.png)

## How to use the sample
Click or tap the 'Clip' button to display the clipping results of the blue graphic and the red outlines.

## How it works
1. Create a `graphicsOverlay` containing an envelope.
2. Create another that contains three other envelopes: one that intersects a part of the first graphic, one that is entirely within it, and another completely outside of it.
3. Create one more `graphicsOverlay`.
4. Create a button object that, when clicked, calls a function that hides the main graphic *only*.
5. In the same function, iterate through the overlay with the three envelopes. `forEach` one, a `Graphic` object is created by calling `GeometryEngine.clip` method, passing in the main graphic' and the current envelope's geometry in the current envelope as arguments.
6. Add each clipped geometry graphic to the empty graphics overlay.

## Relevant API
 - GeometryEngine.clip
 - GraphicsOverlay
 - Graphic
 - Geometry
 - Envelope

## Tags
Geometry, Clip, Envelopes, Graphics, GraphicsOverlay

