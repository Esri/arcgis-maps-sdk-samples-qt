# Graphics overlay (dictionary renderer)

This sample demonstrates applying a dictionary renderer to graphics to display military symbology.

![](screenshot.png)

## How it works

This sample loads a number of point, line, and polygon military elements from an XML file and adds them as graphics to a `GraphicsOverlay`. A `DictionaryRenderer` is applied to the `GraphicsOverlay` in order to display the graphics with MIL-STD-2525D military symbology. When all graphics are created, the map's viewpoint is set to zoom to the full extent of all graphics.

## Features
- MapView
- Map
- BasemapTopographic
- GraphicsOverlay
- DictionaryRenderer
- DictionarySymbolStyle
- SpatialReference
- PointBuilder
- PolygonBuilder
- PolylineBuilder
- Graphic
- GeometryEngine

## Offline Data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[Mil2525d Stylx File](https://www.arcgis.com/home/item.html?id=e34835bf5ec5430da7cf16bb8c0b075c)| `<userhome>`/ArcGIS/Runtime/Data/styles/mil2525d.stylx |
|[MIL-STD-2525D XML Message File](https://arcgisruntime.maps.arcgis.com/home/item.html?id=3db12175479147ea9c89ebaaf3b89996)| `<userhome>`/ArcGIS/Runtime/Data/xml/Mil2525DMessages.xml |
