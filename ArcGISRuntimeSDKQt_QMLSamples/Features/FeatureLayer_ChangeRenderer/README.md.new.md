# Feature layer change renderer

This sample demonstrates how to override and reset a feature layer
renderer.

![](screenshot.png)

## How it works

A `SimpleRenderer` is created with a `SimpleLineSymbol`. When the button
is clicked, the `renderer` property on the `FeatureLayer` is set to the
new `SimpleRenderer`.
