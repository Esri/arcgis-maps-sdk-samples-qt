#Update attributes feature service

This sample demonstrates how to edit attributes of a feature in a feature service.

![](screenshot.png)

##How it works

Features in the map represent parcels and are symbolized based on an attribute specifying the type of damage to the parcels. Selecting a parcel displays a callout. The callout contains information about the type of damage for that parcel. To change the type of damage, select the `information` button in the callout. An editing window appears, allowing you to choose new attribute values. Selecting the `update` button will update the parcel with the new value. 

This is done by first obtaining the parcel feature after it is selected. Once the feature is obtained, the feature's attribute is updated by calling `setAttributeValue`. To update the feature in the feature table, call `updateFeature` and pass in the edited feature. Finally, to update the service, call `applyEdits` to apply the edits to the service.

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
- Point