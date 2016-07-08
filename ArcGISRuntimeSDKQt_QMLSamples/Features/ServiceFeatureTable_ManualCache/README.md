#Service feature table - manual cache

This sample demonstrates how to use a feature service in manual cache mode. In this mode, an app requests features from the remote service by calling the method `populateFromService`.

![](screenshot.png)

##How it works

The sample creates an instance of `ServiceFeatureTable` by supplying the URL to the REST endpoint of the feature service. This feature layer is then supplied with 
the `ServiceFeatureTable` and the `FeatureRequestMode` is set to use `FeatureRequestModeManualCache`. When the `Populate` button is pressed, the `populateFromService` method on the FeatureLayer is called to fetch new features from the service, and they are added to the map.

##Features
- MapView
- Map
- Basemap
- ServiceFeatureTable
- FeatureLayer
- QueryParameters
