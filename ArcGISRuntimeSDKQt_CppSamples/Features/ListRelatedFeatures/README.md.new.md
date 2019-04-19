# List related features

This sample demonstrates how to query a layer for features that are in a
related table.

![](screenshot.png)

## How it works

A `Map` is constructed by passing a URL to a web map stored in ArcGIS
Online. A connection is made to the `mouseClicked` signal of the
`MapView`, and when this signal emits, `selectFeatures` is executed on
the `FeatureLayer`. Once this completes, a `FeatureQueryResult` is
returned, giving access to the `FeatureIterator`. This `FeatureIterator`
contains all of the `Feature` objects selected with the mouse click.
These `Feature` objects are iterated over, and for each object, the
`queryRelatedFeatures` method is called. Once this completes, the
returned `RelatedFeatureQueryResult` gives access to all of the related
features, and the UI is populated with this information.
