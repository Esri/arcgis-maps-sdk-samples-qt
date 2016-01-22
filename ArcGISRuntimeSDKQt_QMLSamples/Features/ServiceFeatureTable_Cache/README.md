#Service feature table - cache

This sample demonstrates how to use a feature table with the FeatureRequestModeOnInteractionCache feature request mode. This mode will cache features locally from the remote feature service. This is the default mode, and will minimize the amount of requests sent to the server, thus lending itself to be the ideal choice for working in a partially connected environment.

![](screenshot.png)

##How it works

The sample creates an instance of ServiceFeatureTable by supplying the URL to the REST endpoint of the feature service. The FeatureLayer is then supplied with the ServiceFeatureTable and added to the Map. Since FeatureRequestModeOnInteractionCache is the default feature request mode, it doesn't need to be explicitly set. 

##Features
- MapView
- Map
- Basemap
- ServiceFeatureTable
- FeatureLayer
- Enums
