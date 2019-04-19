# Feature layer (geodatabase)

This sample demonstrates how to consume an Esri mobile geodatabase by
using a FeatureLayer and a GeodatabaseFeatureTable.

![](screenshot.png)

## How it works

The sample creates an instance of GeodatabaseFeatureTable by supplying
the path to the local mobile geodatabase. The FeatureLayer is then
supplied with the GeodatabaseFeatureTable and added to the Map. The
mobile geodatabase can either be obtained through ArcMap, with the
Create Runtime Content geoprocessing tool, or through a feature service
with the GeodatabaseSyncTask.

## Offline Data

Read more about how to set up the sample’s offline data
[here](http://links.esri.com/ArcGISRuntimeQtSamples).

| Link                                                                                                         | Local Location                                                      |
| ------------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------- |
| [Los Angeles Vector Tile Package](https://www.arcgis.com/home/item.html?id=d9f8ce6f6ac84b90a665a861d71a5d0a) | `<userhome>`/ArcGIS/Runtime/Data/vtpk/LosAngeles.vtpk               |
| [Los Angeles Trailheads](https://www.arcgis.com/home/item.html?id=2b0f9e17105847809dfeb04e3cad69e0)          | `<userhome>`/ArcGIS/Runtime/Data/geodatabase/LA\_Trails.geodatabase |
