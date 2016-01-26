#Change Basemap
This sample shows how to switch between different basemaps in the map. 

![](screenshot.png)

##How it works

When a new item is selected from the combo box, a JavaScript function that determines the currently selected basemap is executed. The map's basemap is replaced by simply calling the writable basemap property, passing in the desired basemap. If there were operational layers present in the map, this would have no effect on them. 

##Features
- Map
- BasemapTopographic
