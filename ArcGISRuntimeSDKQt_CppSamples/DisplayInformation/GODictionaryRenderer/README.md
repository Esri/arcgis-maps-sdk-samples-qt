# Graphics overlay (dictionary renderer)

This sample demonstrates applying a dictionary renderer to graphics, in order to display military symbology without the need for a feature table.

![](screenshot.png)

## How it works

This sample loads a number of point, line, and polygon military elements from an XML file and adds them as graphics to a `GraphicsOverlay`. A `DictionaryRenderer` is applied to the `GraphicsOverlay` in order to display the graphics with MIL-STD-2525D military symbology. When all graphics are created, the map's viewpoint is set to zoom to the full extent of all graphics.

## Features
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
- DictionarySymbolStyle

## Offline Data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[Mil2525d Stylx File](https://www.arcgis.com/home/item.html?id=c78b149a1d52414682c86a5feeb13d30)| `<userhome>`/ArcGIS/Runtime/Data/styles/arcade_style/mil2525d.stylx |
|[MIL-STD-2525D XML Message File](https://arcgisruntime.maps.arcgis.com/home/item.html?id=1e4ea99af4b440c092e7959cf3957bfa)| `<userhome>`/ArcGIS/Runtime/Data/xml/arcade_style/Mil2525DMessages.xml |
