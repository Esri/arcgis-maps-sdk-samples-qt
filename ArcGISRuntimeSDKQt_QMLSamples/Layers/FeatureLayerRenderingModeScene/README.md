# Feature layer rendering mode (scene)

This sample demonstrates the differences between static and dynamic rendering mode for a FeatureLayer in a 3D Scene.

![](screenshot.png)

## How it works
Two `Scenes` are created, each containing several `FeatureLayers`. Prior to loading each `Scene`, several `LoadSettings` are set: `preferredPointFeatureRenderingMode`, `preferredPolygonFeatureRenderingMode`, and `preferredPolylineFeatureRenderingMode`. One `Scene` explicitly sets the rendering mode to be `Static` while the other sets the rendering mode to be `Dynamic`. The rendering mode can change the behavior and performance of the `FeatureLayer` depending on several factors with the underlying data. This sample animates the `SceneView` viewpoint between two `Camera` viewpoints, which helps display the differences between the two modes.

## Features
- FeatureLayer
- LoadSettings
- ServiceFeatureTable
- SceneView
- Scene
- Camera
