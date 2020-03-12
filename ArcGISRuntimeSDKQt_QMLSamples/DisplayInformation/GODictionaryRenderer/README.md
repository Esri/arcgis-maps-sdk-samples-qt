# Graphics overlay (dictionary renderer)

Create graphics using a local mil2525d style file and an XML file with key/value pairs for each graphic.

![](screenshot.png)

## Use case

Use a dictionary renderer on a graphics overlay to display more transient data, such as military messages coming through a local tactical network.

## How to use the sample

Run the sample and view the military symbols on the map.

## How it works

1. Create a new `GraphicsOverlay`.
2. Create a new `DictionaryRenderer` and set it to the graphics overlay.
3. Create a new `DictionarySymbolStyle`.
4. Parse through the XML and create a `Graphic` for each element.
5. Use the `_wkid` key to get the geometry's spatial reference.
6. Use the `_control_points` key to get the geometry's shape.
7. Create a geometry using the shape and spatial reference from above.
8. Create a `Graphic` for each attribute, utilizing its defined geometry.

## Relevant API

* DictionaryRenderer
* DictionarySymbolStyle
* GraphicsOverlay

## Offline data

Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[Mil2525d Stylx File](https://www.arcgis.com/home/item.html?id=c78b149a1d52414682c86a5feeb13d30)| `<userhome>`/ArcGIS/Runtime/Data/styles/arcade_style/mil2525d.stylx |
|[MIL-STD-2525D XML Message File](https://arcgisruntime.maps.arcgis.com/home/item.html?id=1e4ea99af4b440c092e7959cf3957bfa)| `<userhome>`/ArcGIS/Runtime/Data/xml/arcade_style/Mil2525DMessages.xml |

## About the data

The sample opens to a view of the county Wiltshire, United Kingdom. It displays military symbols illustrating a simulated combat situation in the area.

## Tags

defense, military, situational awareness, tactical, visualization