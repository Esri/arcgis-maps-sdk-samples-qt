# Scene layer selection

Identify features in a scene to select.

![](screenshot.png)

## Use case

You can select features to visually distinguish them with a selection color or highlighting. This can be useful to demonstrate the physical extent or associated attributes of a feature, or to initiate another action such as centering that feature in the scene view.

## How to use the sample

Click on a building in the scene layer to select it. Deselect buildings by clicking away from the buildings.

## How it works

1. Create an `ArcGISSceneLayer` passing in the URL to a scene layer service.
2. Use `SceneView::mouseClicked` signal to get the screen tap location `screenPoint`.
3. Call `SceneView::identifyLayerAsync` to identify features in the scene that intersect the tapped screen point.
4. From the resulting `IdentifyLayerResult`, a list of identified `GeoElements` are obtained.
5. Get the first element in the list, checking that it is a feature, and call `ArcGISSceneLayer::selectFeature(feature)` to select it.

## Relevant API

* ArcGISSceneLayer
* Scene
* SceneView

## About the data

This sample shows a [Brest France Buildings Scene](https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_Brest/SceneServer/layers/0) hosted on ArcGIS Online.

## Tags

3D, buildings, identify, model, query, search, select
