# Display a grid

Display coordinate system grids including Latitude/Longitude, MGRS, UTM and USNG on a map view. Also, toggle label visibility and change the color of grid lines and grid labels.

![](screenshot.png)

## Use case

Grids are often used on printed maps, but can also be helpful on digital maps, to identify locations on a map.

## How to use the sample

Tap on the `Change Grid` button in the toolbar to open a settings view. You can select type of grid from `Grid Type` (LatLong, MGRS, UTM and USNG) and modify its properties like label visibility, grid line color, and grid label color.

## How it works
A `MapView` is declared with a `Map` that contains the imagery basemap. The `MapView` is initially set to use the Latitude/Longitude map grid by setting the `MapView.grid` property. Press the button on the bottom to display a settings window, which has several controls that modify the grid. Modifications to the grid include the grid type (MGRS, Lat/Lon, USNG, and UTM), label visibility, grid visibility, label color, grid color, label position, and label format.

## How it works

1. Create an instance of one of the `Grid` types.
2. Grid lines and labels can be styled per grid level with `setLineSymbol(gridLevel, lineSymbol)` and `setTextSymbol(gridLevel, textSymbol)` methods on the grid.
3. The label position can be set with the `labelPosition` property of the grid.
4. For the `LatitudeLongitudeGrid` type, you can specify a label format of `DecimalDegrees` or `DegreesMinutesSeconds`.
5. To set the grid, set `mapView.grid`

## Relevant API

* ArcGISGrid
* Enums.LatitudeLongitudeGridLabelFormatDecimalDegrees
* Enums.LatitudeLongitudeGridLabelFormatDegreesMinutesSeconds
* LatitudeLongitudeGrid
* MapView
* MGRSGrid
* SimpleLineSymbol
* TextSymbol
* USNGGrid
* UTMGrid

## Tags

coordinates, degrees, graticule, grid, latitude, longitude, MGRS, minutes, seconds, USNG, UTM