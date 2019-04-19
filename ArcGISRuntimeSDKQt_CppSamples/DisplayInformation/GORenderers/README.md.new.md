# Add graphics with renderer

This sample demonstrates how to add graphics and set a renderer on
graphic overlays.

![](screenshot.png)

## How it works

The sample creates a `GraphicsOverlay` each for a Point, Line and
Polygon graphic. Then it defines a renderer of type `SimpleRenderer`
with `SimpleMarkerSymbol`, `SimpleLineSymbol` and `SimpleFillSymbol`
symbols respectively.

\#\#Tips

You may modify the sample code so that you can add all the symbols to
the same graphics overlay instead of creating multiple ones. You can do
this by setting a symbol on each graphic and adding to the graphics
overlay.
