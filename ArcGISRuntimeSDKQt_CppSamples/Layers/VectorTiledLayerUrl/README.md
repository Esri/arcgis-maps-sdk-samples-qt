# Vector tiled layer (URL)

This sample demonstrates how to use a vector tiled layer from an ArcGIS Online service as a basemap.

![](screenshot.png)

## How it works

When a new item is selected from the combo box, an invokable C++ function that determines the currently selected vector tiled layer basemap is executed. The map's basemap is replaced by simply calling its `setBasemap` method, passing in the desired basemap.

## Features
- MapView
- Map
- Basemap
- ArcGISVectorTiledLayer
