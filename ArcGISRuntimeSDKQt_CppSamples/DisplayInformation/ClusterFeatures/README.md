# Cluster Features

Display a webmap with a point feature layer that supports clustering.

![](screenshot.png)

## Use case

asdfasdfasdfa

## How to use the sample

Pan and zoom the map to view how point features are clustered. Toggle clustering on and off to view the indiviual points that make up the clustered features. Click on a point to view information about a cluster, or an individual power plant.

## How it works

1. Create a map from a webmap `PortalItem`.
2. Get the cluster enabled layer from the map's operational layers.
3. Get the `FeatureReduction` from the feature layer and call `setEnabled(bool enabled)` to enable or disable clustering on the feature layer.
4. When the user clicks on the map, call `identifyFeatureLayerAsync` on the feature layer and pass in the map click location.
5. Get the `GeoElement` from the resulting `IdentifyLayerResult` and use its attributes to set `MapView::CalloutData`.
6. Check to see if the `GeoElement` is an `AggregateGeoElement` and set the callout data accordingly.

## Relevant API

- AggregateGeoElement
- CalloutData
- FeatureLayer
- FeatureReduction
- GeoElement
- IdentifyLayerResult

## Tags

aggregate, cluster, group, merge, normalize, summarize, bin, reduce

