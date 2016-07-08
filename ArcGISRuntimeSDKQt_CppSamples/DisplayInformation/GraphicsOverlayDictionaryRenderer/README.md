#Graphics overlay (dictionary renderer)

This sample demonstrates applying a dictionary renderer to graphics to display military symbology.

![](screenshot.png)

##How it works

This sample loads a number of point, line, and polygon military elements from an XML file and adds them as graphics to a `GraphicsOverlay`. A `DictionaryRenderer` is applied to the `GraphicsOverlay` in order to display the graphics with MIL-STD-2525D military symbology. When all graphics are created, the map's viewpoint is set to zoom to the full extent of all graphics.

##Features
- Basemap
- DictionaryRenderer
- Envelope
- Geometry
- GeometryEngine
- Graphic
- GraphicListModel
- GraphicsOverlay
- Map
- MapQuickView
- MultipartBuilder
- Point
- PolygonBuilder
- PolylineBuilder
- SpatialReference
- SymbolDictionary

##Offline Data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[Mil2525d Stylx File](https://www.arcgis.com/home/item.html?id=4581a9a92c214240a79e15482a2e8349)| `<userhome>`/ArcGIS/Runtime/Data/styles/mil2525d.stylx |
|[MIL-STD-2525D XML Message File](https://www.arcgis.com/home/item.html?id=623382e0113d40698538f249e3bcb1c0)| `<userhome>`/ArcGIS/Runtime/Data/xml/Mil2525DMessages.xml |
