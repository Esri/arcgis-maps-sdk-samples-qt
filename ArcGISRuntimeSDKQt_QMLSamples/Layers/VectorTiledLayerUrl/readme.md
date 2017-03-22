# Vector Tiled Layer Url

This sample demonstrates how to use a vector tiled layer from an ArcGIS Online service as a basemap.

![](screenshot.png)

## How it works

When a new item is selected from the combo box, a JavaScript
function that determines the currently selected vector tiled layer
basemap is executed. The map's basemap is replaced by setting `basemap` 
property to the desired basemap. If there were operational
layers present in the map, this would have no effect on them.

## Features
- MapView
- Map
- Basemap
- ArcGISVectorTiledLayer

