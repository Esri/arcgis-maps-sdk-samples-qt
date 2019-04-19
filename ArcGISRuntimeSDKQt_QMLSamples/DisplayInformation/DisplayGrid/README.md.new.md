# Display a grid

This sample demonstrates how to display grids such as MGRS, Lat/Lon,
USNG, and UTM on a map. In addition, this sample demonstrates how to
configure the look and feel of the grid.

![](screenshot.png)

## How it works

A `MapView` is declared with a `Map` that contains the imagery basemap.
The `MapView` is initially set to use the Latitude/Longitude map grid by
setting the `MapView::grid` property. Press the button on the bottom to
display a settings window, which has several controls that modify the
grid. Modifications to the grid include the grid type (MGRS, Lat/Lon,
USNG, and UTM), label visibility, grid visibility, label color, grid
color, label position, and label format.
