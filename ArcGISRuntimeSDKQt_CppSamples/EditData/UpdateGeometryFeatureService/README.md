#Update geometry feature service

The sample demonstrates how to update geometry of a feature in a feature service.

![](screenshot.png)

##How it works

Tapping on a feature on the map selects and highlights the feature. Clicking on another location on the map moves the feature to the new location. This is done by first obtaining the feature after it is selected. Once the feature is obtained, the feature's geometry is updated by setting the geometry to the new map point. To update the feature in the feature table, call `updateFeature`, and pass in the edited feature. Finally, to update the service, call the `applyEdits` method, and this will apply the geometry edits to the service.

##Features
- MapView
- Map
- Basemap
- Viewpoint
- SpatialReference
- ServiceFeatureTable
- FeatureLayer
- Feature
- ArcGISFeature
- FeatureEditResult
- FeatureQueryResult