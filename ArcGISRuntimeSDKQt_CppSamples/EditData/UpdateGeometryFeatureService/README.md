#Update geometry (feature service)

This sample demonstrates how to update geometry of a feature in a feature service.

![](capture.png)

##How it works

This sample calls identify on the MapView in order to obtain features from the touch event. Once a feature is selected, its geometry is updated by calling setGeometry, and passing in the point obtained from the mousePressRelease signal on the MapView. To update the feature in the feature table, call updateFeature and pass in the feature with the edited geometry. The updateFeatureCompleted signal fires once this completes. Finally, the edits are applied to the feature service by calling the applyEdits method on the ServiceFeatureTable.


##Features
- MapView
- Map
- Basemap
- ServiceFeatureTable
- FeatureLayer
- Point
- Feature