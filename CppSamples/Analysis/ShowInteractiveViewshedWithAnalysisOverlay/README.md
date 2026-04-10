# Show interactive viewshed with analysis overlay

Perform an interactive viewshed analysis to determine visible and non-visible areas from a given observer position.

![](screenshot.png)

## How to use the sample

The sample loads with a viewshed analysis initialized from an elevation raster covering the Isle of Arran, Scotland. Transparent green shows the area visible from the observer position, and grey shows the non-visible areas. Move the observer position by clicking and dragging over the island to interactively evaluate the viewshed result and display it in the analysis overlay. Alternatively, click on the map to see the viewshed from the clicked location. Use the control panel to explore how the viewshed analysis results change when adjusting the observer elevation, target height, maximum radius, field of view, heading and elevation sampling interval. As you move the observer and update the viewshed parameters, the analysis overlay refreshes to show the evaluated viewshed result.

## How it works

1. Create an `Map` and set it on an `MapView`.
2. Add a `GraphicsOverlay` to draw the observer point and an `AnalysisOverlay` to the map view.
3. Create a `ContinuousField` from a raster file containing elevation data.
4. Create and configure `ViewshedParameters`, passing in an `Point` as the observer position for the viewshed.
5. Create a `ContinuousFieldFunction` from the continuous field.
6. Create a `ViewshedFunction` using the continuous field function and viewshed parameters, then convert it to a `DiscreteFieldFunction`.
7. Create a `ColormapRenderer` from a `Colormap` with colors that represent visible and non-visible results.
8. Create a `FieldAnalysis` from the discrete field function and colormap renderer, then add it to the `AnalysisOverlay`'s collection of analysis objects to display the results. As parameter values change, the result is recalculated and redrawn automatically.

## Relevant API

- AnalysisOverlay
- Colormap
- ColormapRenderer
- ContinuousField
- ContinuousFieldFunction
- FieldAnalysis
- ViewshedFunction
- ViewshedParameters

## Offline data

The sample uses a [10m resolution digital terrain elevation raster of the Isle of Arran, Scotland](https://www.arcgis.com/home/item.html?id=aa97788593e34a32bcaae33947fdc271)
(Data Copyright Scottish Government and SEPA (2014)).

## Tags

analysis overlay, elevation, field analysis, interactive, raster, spatial analysis, terrain, viewshed, visibility

