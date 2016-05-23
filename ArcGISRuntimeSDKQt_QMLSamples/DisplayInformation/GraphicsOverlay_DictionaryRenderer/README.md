#Graphics overlay (dictionary renderer)

This sample demonstrates applying a dictionary renderer to graphics, in order to display military symbology without the need for a feature layer.

![](screenshot.png)

##How it works

The sample loads a number of point, line, and polygon military elements from an XML file and adds them as graphics to a `GraphicsOverlay`. A `DictionaryRenderer is applied to the `GraphicsOverlay` in order to display the graphics with MIL-STD-2525D military symbology. When all graphics are created, the map's viewpoint is set to zoom to the full extent of all graphics, buffered slightly to nicely display all graphics.

##Features
- MapView
- Map
- BasemapTopographic
- GraphicsOverlay
- DictionaryRenderer
- SymbolDictionary
- SpatialReference
- PointBuilder
- PolygonBuilder
- PolylineBuilder
- Graphic
- GeometryEngine
