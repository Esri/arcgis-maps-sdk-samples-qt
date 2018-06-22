# Min Max Scale

This demonstrates how to set the minimum and maximum scale of an ArcGIS Map.

![](screenshot.png)

## How to use the sample
Use the mousewheel to zoom in and out of the map. Notice that the scale of which you can do that is limited by the application.

## How it works
1. Create a MapView QML object, and create a Map child. Set the basemap and dimensions to whatever you like.
2. Map QML objects have two properties called `minScale` and `maxScale`: the latter restricts how far you can zoom out, and the former how far you can zoom in. Individually, set these properties to the desired range (`maxScale` must be less than `minScale`, though).

## Relevant API
 - minScale
 - maxScale

## Tags
Maps, 2D, scale, minScale, maxScale

