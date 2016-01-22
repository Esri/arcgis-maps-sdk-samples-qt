#Service feature table - manual cache

This sample demonstrates how to use a feature service in manual cache mode. 

![](screenshot.png)

##How it works

The sample creates an instance of ServiceFeatureTable by supplying the URL to the REST endpoint of the feature service. The FeatureLayer is then supplied with 
the ServiceFeatureTable and the FeatureRequestMode is set to use FeatureRequestModeManualCache. When the "Populate" button is pressed, The populateFromService method on the FeatureLayer
is called to fetch new features from the service, and they are added to the map.

##Features
- MapView
- Map
- Basemap
- ServiceFeatureTable
- FeatureLayer
- QueryParameters
