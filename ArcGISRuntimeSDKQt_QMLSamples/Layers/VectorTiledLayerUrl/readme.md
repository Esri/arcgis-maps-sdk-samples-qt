#Vector Tiled Layer Url

This sample uses Vector Tiled Layer basemap from an ArcGIS Online service URL. It shows how to creates an ArcGISVectorTiledLayer from an ArcGIS Online service URL and bind that to a  Basemap.

![](screenshot.png)

##How it works

When a new item is selected from the combo box, a JavaScript function that determines the currently selected Vector Tiled Layer basemap is executed. The map's basemap is replaced by simply calling the writable basemap property, passing in the desired basemap. If there were operational layers present in the map, this would have no effect on them. 


##Features
- MapView
- Map
- Basemap
- ArcGISVectorTiledLayer

