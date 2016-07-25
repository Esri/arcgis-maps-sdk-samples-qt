#Delete features feature service

This sample demonstrates how to delete a feature from a feature service.

![](screenshot.png)

##How it works

Tapping on a feature displays a callout. To delete the feature from the feature service, tap the delete button in the callout. This button will invoke the `deleteFeature` method from the feature table, at which point the feature will be deleted from the table. To update the service, call the `applyEdits` method, and this will apply the edits to the service, and the feature will be deleted from the service.

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
