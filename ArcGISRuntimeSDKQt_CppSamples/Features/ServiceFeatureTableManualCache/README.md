#Service feature table - manual cache

This sample demonstrates how to use a feature service in manual cache mode. In this mode, an app requests features from the remote service by calling the method `populateFromService`.

![](screenshot.png)

##How it works

The sample creates an instance of `ServiceFeatureTable` by supplying the URL to the REST endpoint of the feature service. The feature layer is then supplied with the `ServiceFeatureTable` and added to the Map. Finally, the `featureRequestMode` is set to use ManualCache, and `populateFromService` is called to fetch new features from the service.

##Features
- MapView
- Map
- Basemap
- ServiceFeatureTable
- FeatureLayer
- QueryParameters