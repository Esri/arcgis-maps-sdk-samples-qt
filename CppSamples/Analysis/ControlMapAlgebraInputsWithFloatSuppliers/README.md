# Control map algebra inputs with float suppliers

Use float suppliers to dynamically update numeric inputs to a map algebra expression and refresh analysis results as the input values change.

![](screenshot.png)

## Use case

Adjust a slope range to explore terrain. The sample initially highlights slopes from 0 to 10 degrees over an elevation field. Change the minimum and maximum values from 0 to 85 degrees to update the displayed slope results.

## How it works

1. Create a `ContinuousField` from the Meribel elevation raster and wrap it in a `ContinuousFieldSupplier`.
2. Derive a slope function and create `FloatSupplier` objects for the minimum and maximum slope values.
3. Compare the slope function with both suppliers, combine the comparisons into a boolean selection, and mask the slope function.
4. Display the elevation and masked slope functions as `FieldAnalysis` objects in an `AnalysisOverlay`.
5. Update the supplier values from the range slider. The existing analysis function recalculates as the values change.

## Relevant API

* `AnalysisOverlay`
* `BooleanFieldFunction`
* `ContinuousField`
* `ContinuousFieldFunction`
* `ContinuousFieldSupplier`
* `FieldAnalysis`
* `FloatSupplier`
* `StretchRenderer`

## About the data

The sample uses a 1 m resolution digital terrain elevation raster of the area over Meribel in the French Alps, in UTM zone 32N. The data is available from [ArcGIS Online](https://www.arcgis.com/home/item.html?id=e8fe90cd623b48dfa683f448bef65e70).

## Tags

elevation, field analysis, float supplier, map algebra, raster, slope, terrain