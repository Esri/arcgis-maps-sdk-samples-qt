#Feature layer (geodatabase)

This sample demonstrates how to consume an Esri mobile geodatabase by using a FeatureLayer and a GeodatabaseFeatureTable.

![](screenshot.png)

##How it works

The sample creates an instance of GeodatabaseFeatureTable by supplying the path to the local mobile geodatabase. The FeatureLayer is then supplied with the GeodatabaseFeatureTable and added to the Map. The mobile geodatabase can either be obtained through ArcMap, with the Create Runtime Content geoprocessing tool, or through a feature service with the GeodatabaseSyncTask.

##Features
- MapView
- Map
- Basemap
- GeodatabaseFeatureTable
- FeatureLayer
