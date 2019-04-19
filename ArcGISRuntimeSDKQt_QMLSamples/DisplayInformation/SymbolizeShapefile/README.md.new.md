# Symbolize a shapefile

This sample demonstrates how to override the default renderer of a
shapefile when displaying with a FeatureLayer.

![](screenshot.png)

## How it works

A `ShapefileFeatureTable` is created by setting the path to a shapefile.
The shapefile is then displayed by creating a `FeatureLayer` and setting
the feature table property to the `ShapefileFeatureTable`. This layer is
added to a `Map` and the default black and gray symbology is applied. To
override the default symbology, a `SimpleRenderer` is created. A
`SimpleRenderer` is used to apply the same symbol to every feature. In
this case, a yellow `SimpleFillSymbol` with a red `SimpleLineSymbol` as
an outline is used. The renderer is set on the `FeatureLayer`, and the
layer’s symbology updates.

## Offline Data

Read more about how to set up the sample’s offline data
[here](http://links.esri.com/ArcGISRuntimeQtSamples).

| Link                                                                                                     | Local Location                                        |
| -------------------------------------------------------------------------------------------------------- | ----------------------------------------------------- |
| [Aurora Subdivions Shapefile](https://www.arcgis.com/home/item.html?id=d98b3e5293834c5f852f13c569930caa) | `<userhome>`/ArcGIS/Runtime/Data/shp/Subdivisions.shp |
