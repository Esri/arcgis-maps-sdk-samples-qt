# Read geopackage

This sample demonstrates how to read rasters and feature tables from geoPackages to show them as layers in a map.

![](screenshot.png)

## How to use the sample
Use the top-left menu to select individual geopackage layers to add to the map. Unselect them to hide them.

## How it works
1. Create an empty menu UI using QML, along with empty `listView` and `listModel` objects.
2. Create a `GeoPackage` object within the `listView` and load it locally, or download if remote.
3. Iterate through the geopackage, once for the `featureTables` layers and once for the `raster` layers. In each iteration, create an `ArcGISRuntime` Object that constructs the appropriate layer.
4. Still within the iteration, create a `QVariantMap` that has "lyr" (layer object) and "name" (layer name) keys. Append these to a `QVariantMap`.
5. Within `listView`, ensure the `delegate` takes the `QVariantList` as the `model`.
6. For each `Item` in the `model`, create a row and a switch as child components that, when `checked`, appends the current `lyr` `Item` to the `map`'s `operationalLayers`. When `unchecked`, it hides the layer.

## Relevant API
 - Esri::ArcGISRuntime::GeoPackage

## Tags
GeoPackage, Maps, data, UI, listView
