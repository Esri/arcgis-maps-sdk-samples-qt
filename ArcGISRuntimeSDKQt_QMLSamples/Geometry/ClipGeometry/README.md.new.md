# Clip Geometry

This sample demonstrates how to clip a geometry with an envelope using
the GeometryEngine.

![](screenshot.png)

## How to use the sample

Click or tap the ‘Clip’ button to display the clipping results of the
blue graphic and the red outlines.

## How it works

1.  Create a `GraphicsOverlay` and add a `Graphic` containing an
    envelope.
2.  Create another `GraphicsOverlay` that contains three other
    envelopes: one that intersects a part of the first graphic, one that
    is entirely within it, and another completely outside of it.
3.  Create a third `GraphicsOverlay` but add no graphics to it.
4.  Create a Button that, when clicked, calls a function that hides the
    main graphic. In the same function, iterate through the
    `GraphicsOverlay` that contains the three envelopes and call
    `GeometryEngine.clip` for each `Envelope`, by passing in the main
    graphic’s `Geometry` and the current `Envelope` as arguments.
5.  Add each clipped `Geometry` graphic to the empty `GraphicsOverlay`
    to visualize the clipped geometries.
