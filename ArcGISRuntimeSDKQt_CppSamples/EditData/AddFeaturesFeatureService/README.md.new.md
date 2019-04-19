# Add features (feature service)

This sample demonstrates how to add features to a feature service. Click
or tap on a location in the map view to add a feature at that location.

![](screenshot.png)

## How it works

This workflow is accomplished through obtaining the map point from the
mouse event on the map view. Once the map point is obtained, attributes
must be created. This is done by creating a JSON of key/value pairs for
the field name and field value. Once the geometry and attributes are
ready, the feature can be created through the `createFeature` method. To
add the feature to the feature table, call `addFeature`, and pass in the
new feature. Finally, to update the service, call the `applyEdits`
method, and this will apply the feature edits to the service.
