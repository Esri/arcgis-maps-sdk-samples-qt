#Feature layer (dictionary renderer)

This sample demonstrates how to display a `FeatureLayer` with military symbology, using ArcGIS Runtime's `DictionaryRenderer`.

![](screenshot.png)

##How it works

This sample loads a number of point, line, and polygon feature tables from a Runtime geodatabase. For each feature table, a `FeatureLayer` is created, and a `DictionaryRenderer` object is created and applied to the layer. Note that each layer needs its own renderer, though all renderers can share the SymbolDictionary, in which case all layers will use the same symbology specification (MIL-STD-2525D in the case of this sample). Each layer is added to the map, and when all layers are loaded, the map's viewpoint is set to zoom to the full extent of all feature layers.

##Features
- MapView
- Map
- BasemapTopographic
- SymbolDictionary
- DictionaryRenderer
- Geodatabase
- FeatureLayer
- GeometryEngine
