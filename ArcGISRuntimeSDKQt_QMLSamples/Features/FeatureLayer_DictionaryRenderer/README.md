#Feature layer (dictionary renderer)

This sample demonstrates how to display a `FeatureLayer` with military symbology, using ArcGIS Runtime's `DictionaryRenderer`.

![](screenshot.png)

##How it works

This sample loads a number of point, line, and polygon feature tables from a Runtime geodatabase. For each feature table, a `FeatureLayer` is created, and a `DictionaryRenderer` object is created and applied to the layer. Note that each layer needs its own renderer, though all renderers can share the DictionarySymbolStyle, in which case all layers will use the same symbology specification (MIL-STD-2525D in the case of this sample). Each layer is added to the map, and when all layers are loaded, the map's viewpoint is set to zoom to the full extent of all feature layers.

##Features
- MapView
- Map
- BasemapTopographic
- DictionarySymbolStyle
- DictionaryRenderer
- Geodatabase
- FeatureLayer
- GeometryEngine

##Offline Data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[Mil2525d Stylx File](https://www.arcgis.com/home/item.html?id=e34835bf5ec5430da7cf16bb8c0b075c)| `<userhome>`/ArcGIS/Runtime/Data/styles/mil2525d.stylx |
|[Military Overlay geodatabase](https://www.arcgis.com/home/item.html?id=e0d41b4b409a49a5a7ba11939d8535dc)| `<userhome>`/ArcGIS/Runtime/Data/geodatabase/militaryoverlay.geodatabase |
