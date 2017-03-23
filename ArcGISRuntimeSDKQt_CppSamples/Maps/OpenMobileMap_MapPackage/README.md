# Open mobile map (map package)

This sample demonstrates how to open and display a map from a Mobile Map Package.

![](screenshot.png)

## How it works

This sample takes a Mobile Map Package that was created in ArcGIS Pro, and displays a `Map` from within the package in a `MapView`. This is accomplished by calling `load` on the `MobileMapPackage`, and waiting for its load status to be Completed. Once the package is loaded, you can access its maps, and assign one of the maps to be viewed in the `MapView`.

## Features
- MapView
- MobileMapPackage

## Offline Data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[Yellowstone Mobile Map Package](https://www.arcgis.com/home/item.html?id=e1f3a7254cb845b09450f54937c16061)| `<userhome>`/ArcGIS/Runtime/Data/mmpk/Yellowstone.mmpk |
