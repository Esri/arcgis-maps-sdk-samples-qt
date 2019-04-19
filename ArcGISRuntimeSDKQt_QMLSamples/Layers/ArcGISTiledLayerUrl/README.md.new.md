# ArcGIS tiled layer (URL)

This sample demonstrates how to display a tiled map service.

![](screenshot.png)

## How it works

An `ArcGISTiledLayer` is created by setting its `url` property to the
URL of the map service endpoint. The `ArcGISTiledLayer` is nested within
a basemap, thus becoming one of the basemap’s layers. The basemap is set
as the basemap of the map, and the map is set as the map for the map
view. Note how default properties in QML make this nesting easier to
code.
