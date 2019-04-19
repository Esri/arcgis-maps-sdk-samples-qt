# Identify layers

This sample demonstrates how to identify features on a map across the
different layers in the map.

![](screenshot.png)

## How it works

The `GeoView::identifyLayers` method is executed by passing in the
screen coordinates of a mouse click, along with a tolerance and whether
to return popups. The `identifyLayers` method differs from the
`identifyLayer` method because it performs an identify on every layer in
the map or scene - not just one layer. Since the process is executed for
several different layers, a list of `IdentifyLayerResult` is returned.
This particular example has a map with a `FeatureLayer` and an
`ArcGISMapImageLayer` in the operational layer list. A `FeatureLayer`
will have all returned GeoElements available at the root level, but the
`ArcGISMapImageLayer` could contain sublayers, each of which could
contain additional sublayers. Because of this, a depth first search is
used to find the number of results in the sublayers. The layer name and
the total count is displayed in a Message Dialog.
