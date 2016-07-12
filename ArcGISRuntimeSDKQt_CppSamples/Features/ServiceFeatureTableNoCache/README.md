#Service feature table - no cache

This sample demonstrates how to use a feature table in on interaction no cache mode. In this mode, an app requests features from the remote service and does not cache them. This means that new features are requested from the service each time the viewpoint's visible extent changes.

![](screenshot.png)

##How it works

The sample creates an instance of `ServiceFeatureTable` by supplying the URL to the REST endpoint of the feature service. The feature layer is then supplied with the `ServiceFeatureTable` and added to the map. Finally, the OnInteractionNoCache feature request mode is set.

##Features
- MapView
- Map
- Basemap
- ServiceFeatureTable
- FeatureLayer