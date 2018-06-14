# Scene layer selection

This sample demonstrates how to identify and select `GeoElements` in an `ArcGISSceneLayer`. Performing a select will visually distinguish the selected `GeoElement` by displaying a cyan color overlay.

![](screenshot.png)

## How to use the sample
Tap on an element in the `ArcGISSceneLayer` and the associated GeoElement will be selected and highlighted. Tap elsewhere and the selection is cleared and any new identified elements will be selected.

## How it works
- Create an `ArcGISSceneLayer` by passing in the URL to a scene layer service.
- Use `SceneView::mouseClicked` signal to get the screen tap location screen point.
- Call `SceneView::identifyLayer` to identify features in the scene that intersect the tapped screen point.
- From the resulting `IdentifyLayerResult`, a list of identified `GeoElements` are obtained. The first element in the list is obtained and `ArcGISSceneLayer::selectFeature(feature)` is used to select it.

## Relevant API
- ArcGISSceneLayer
- ArcGISSceneLayer::selectFeature
- SceneView::identifyLayer
- IdentifyLayerResult
