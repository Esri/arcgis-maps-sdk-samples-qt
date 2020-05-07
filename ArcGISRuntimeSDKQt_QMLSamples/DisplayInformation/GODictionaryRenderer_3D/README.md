# Graphics overlay (dictionary renderer) 3D

This sample demonstrates applying a dictionary renderer to a graphics overlay in a 3D scene to display military symbology.

![](screenshot.png)

## Use case

Use a dictionary renderer on a graphics overlay to display more transient data, such as military messages coming through a local tactical network.

## How it works

The sample loads a number of point military elements from an XML file and adds them as graphics to a `GraphicsOverlay`. A `DictionaryRenderer` is applied to the `GraphicsOverlay` in order to display the graphics with MIL-STD-2525D military symbology. The `GraphicsOverlay`'s `renderingMode` parameter is set to `GraphicsRenderingModeDynamic`, so that point features are displayed with billboarded symbols facing the user (a developer can set `renderingMode` to `GraphicsRenderingModeStatic` if desired to instead drape the symbols on the surface). When all graphics are created, the 3D scene's viewpoint is set to zoom to the full extent of all graphics.

## Relevant API

* DictionaryRenderer
* DictionarySymbolStyle
* GraphicsOverlay

## Offline Data

Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[Mil2525d Stylx File](https://www.arcgis.com/home/item.html?id=c78b149a1d52414682c86a5feeb13d30)| `<userhome>`/ArcGIS/Runtime/Data/styles/arcade_style/mil2525d.stylx |
|[MIL-STD-2525D XML Message File](https://arcgisruntime.maps.arcgis.com/home/item.html?id=1e4ea99af4b440c092e7959cf3957bfa)| `<userhome>`/ArcGIS/Runtime/Data/xml/arcade_style/Mil2525DMessages.xml |

## About the data

The sample opens to a view of the county Wiltshire, United Kingdom. It displays military symbols illustrating a simulated combat situation in the area.

## Tags

defense, military, situational awareness, tactical, visualization