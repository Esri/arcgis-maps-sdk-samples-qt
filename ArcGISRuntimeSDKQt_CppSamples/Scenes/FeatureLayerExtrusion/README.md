# Feature Layer Extrusion

This sample demonstrates how to apply extrusion to a renderer on a feature layer.

![](screenshot.png)


## How it works
Create a `ServiceFeatureTable` from a web service.
Set the `ServiceFeatureTable` to a `FeatureLayer` and `.setRenderingMode(FeatureRenderingMode::Dynamic)`.
When definining the `FeatureLayer`'s `Renderer`, remember to `.setExtrusionMode(...)` on the `SceneProperties`.
Finally, also on `SceneProperties`, use `setExtrusionExpression("[SOME_FIELD]")` to a `Field` from the `ServiceFeatureTable`.


## Features
* FeatureLayer
* ExtrusionMode
* Renderer
* SceneProperties
