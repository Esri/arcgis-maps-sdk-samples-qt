# Simple renderer

Display common symbols for all graphics in a graphics overlay with a renderer.

![](screenshot.png)

## Use case

Graphics are often used to display data that changes location regularly because they are held in memory instead of being persisted to the map. When a number of graphics are required to share the same style, a simple renderer can be applied to the graphics overlay containing the graphics. For example, the location of mobile taxis from a same company could be plotted on a map with each taxi marked by a relevant symbol.

## How to use the sample

The sample loads with a predefined simple renderer, which displays a red cross simple marker symbol for the graphics in the graphics overlay.

## How it works
A `GraphicsOverlay` is added to the `MapView`. A `SimpleRenderer` is created in the `GraphicsOverlay` which sets a red cross `SimpleMarkerSymbol` as its symbol. Three `Graphic`s are added using `Point`s.
1.  Create a `GraphicsOverlay`.
2.  Create a number of `Point` objects with x, y and spatial reference parameters.
3.  Create a `SimpleMarkerSymbol` passing in a `SimpleMarkerSymbolStyle`, color and size as parameters.
4.  Create a `SimpleRenderer` and set the `SimpleMarkerSymbol` as its symbol.
6.  Create a number of `Graphic` objects with the points initiated in step 2.

## Relevant API

* Graphic
* GraphicsOverlay
* Point
* SimpleMarkerSymbol
* SimpleRenderer

## Additional information

Renderers are used to display graphics that don't already have a symbol set. A renderer will not override a graphic's symbol if one has been set.

## Tags

graphics, marker, renderer, symbol, symbolize, symbology