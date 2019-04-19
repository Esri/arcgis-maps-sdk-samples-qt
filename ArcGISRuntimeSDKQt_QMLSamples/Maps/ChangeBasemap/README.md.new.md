# Change Basemap

This sample demonstrates how to change a map’s basemap.

![](screenshot.png)

## How it works

When a new item is selected from the combo box, a JavaScript function
that determines the currently selected basemap is executed. The map’s
basemap is replaced by simply calling the writable basemap property,
passing in the desired basemap. If there were operational layers present
in the map, this would have no effect on them.
