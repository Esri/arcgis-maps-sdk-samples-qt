# Feature Layer Extrusion

This sample demonstrates how to apply extrusion to a renderer on a
feature layer.

![](screenshot.png)

## How it works

Create a `ServiceFeatureTable` from a web service. Set the
`ServiceFeatureTable` to a `FeatureLayer` and set the rendering mode to
`Enums.FeatureRenderingModeDynamic`. When definining the FeatureLayer’s
`Renderer`, remember to set the extrusion mode on the `SceneProperties`.
Finally, also on `SceneProperties`, use set the extrusion expression to
(`"[SOME_FIELD]"`) to a `Field` from the `ServiceFeatureTable`.
