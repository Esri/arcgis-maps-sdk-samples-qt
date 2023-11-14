# Display a 3D tiles layer

Add a layer to visualize 3D tiles data that conforms to the OGC 3D Tiles specification.

![](screenshot.png)

## Use case

The [3D Tiles Open Geospatial Consortium (OGC) specification](https://www.ogc.org/standard/3DTiles) defines a spatial data structure and a set of tile formats designed for streaming and rendering massive heterogeneous 3D geospatial content. A 3D tiles layer (`Ogc3dTilesLayer`) displays data from a tileset (`.json`) or from a 3D tiles archive format (`.3tz`).

The 3D tiles layer added to a scene can assist in performing visual analysis. The sample demonstrates how to present the results of a line of sight analysis (location) using the added 3D tiles layer.

## How to use the sample

When loaded, the sample will display a scene with an `Ogc3dTilesLayer`, along with a pair of observer-target points for the line of sight analysis. A [line of sight analysis (location)](https://github.com/Esri/arcgis-maps-sdk-samples-qt/tree/main/ArcGISRuntimeSDKQt_CppSamples/Analysis/LineOfSightLocation) can be used to assess whether a view is obstructed between an observer and a target. Pan around and zoom in to observe the scene of the `Ogc3dTilesLayer`. Click to update the target location.

## How it works

1. Create a scene.
2. Create an `Ogc3dTilesLayer` using the path to the local `.3tz`
3. Add the layer to the scene's operational layers collection.
4. Create an `AnalysisOverlay` and add it to the scene view.
5. Create a `LocationLineOfSight` with initial observer and target locations and add it to the analysis overlay.
6. Connect to the `SceneQuickView::mouseClicked` slot to capture mouse movement. 
7. Turn the screen point into a scene point with `sceneView::screenToLocation(screenPoint)`.
8. Update the target location with `LineOfSight::setTargetLocation(scenePoint)`.

## Relevant API

* AnalysisOverlay
* LocationLineOfSight
* SceneView

## Offline data

Link | Local Location
---------|-------|
|[Stuttgart 3D Tiles]()|`<userhome>/ArcGIS/Runtime/Data/Stuttgart.3tz`|

## Tags

3D Tiles, line of sight analysis, Ogc3dTilesLayer, scene

