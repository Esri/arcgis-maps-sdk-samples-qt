#Delete features (feature service)

This sample demonstrates how to delete features from a feature service using a ServiceFeatureTable.

![](capture.png)

##How it works

This sample calls identify on the MapView in order to obtain features from the touch event. Once a feature is selected, it is deleted from the ServiceFeatureTable by calling deleteFeature, and passing in the feature that was obtained through the identify process. The deleteFeatureCompleted signal will fire once the feature is successfully deleted from the feature table. Finally, the edits are applied to the feature service by calling the applyEdits method on the ServiceFeatureTable.

##Features
- MapView
- Map
- Basemap
- ServiceFeatureTable
- FeatureLayer
- Point
- Feature