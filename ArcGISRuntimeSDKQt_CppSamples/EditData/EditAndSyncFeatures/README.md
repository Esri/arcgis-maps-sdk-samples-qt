# Edit and sync features

Synchronize offline edits with a feature service.

![](screenshot.png)

## Use case

A survey worker who works in an area without an internet connection could take a geodatabase of survey features offline at their office, make edits and add new features to the offline geodatabase in the field, and sync the updates with the online feature service after returning to the office.

## How to use the sample

Pan and zoom to position the red rectangle around the area you want to take offline. Click "Generate geodatabase" to take the area offline. When complete, the map will update to only show the offline area. To edit features, click to select a feature, and click again anywhere else on the map to move the selected feature to the clicked location. To sync the edits with the feature service, click the "Sync geodatabase" button.

## How it works

1. Create a `GeodatabaseSyncTask` from a URL to a feature service.
2. Create `GenerateGeodatabaseParameters`, passing in an `Envelope` extent as the parameter.
3. Create a `GenerateGeodatabaseJob` from the `GeodatabaseSyncTask` using `generateGeodatabase(...)`, passing in the parameters and a path to where the geodatabase should be downloaded locally.
4. Start the job and get the result `Geodatabase`.
5. Load the geodatabase and get its feature tables. Create feature layers from the feature tables and add them to the map's operational layers collection.
6. Create `SyncGeodatabaseParameters` and set the sync direction.
7. Create a `SyncGeodatabaseJob` from `GeodatabaseSyncTask` using `syncGeodatabase(...)` passing in the parameters and geodatabase as arguments.
8. Start the sync job to synchronize the edits.

## Relevant API

* FeatureLayer
* FeatureTable
* GenerateGeodatabaseJob
* GenerateGeodatabaseParameters
* GeodatabaseSyncTask
* SyncGeodatabaseJob
* SyncGeodatabaseParameters
* SyncLayerOption

## Offline Data
To set up the sample's offline data, see the [Use offline data in the samples](https://github.com/Esri/arcgis-runtime-samples-qt#use-offline-data-in-the-samples) section of the Qt Samples repository overview.

Link | Local Location
---------|-------|
|[San Francisco Streets TPKX](https://www.arcgis.com/home/item.html?id=e4a398afe9a945f3b0f4dca1e4faccb5)| `<userhome>`/ArcGIS/Runtime/Data/tpkx/SanFrancisco.tpkx |

## About the data

The basemap uses an offline tile package of San Francisco. The online feature service has features with wildfire information.

## Tags

feature service, geodatabase, offline, synchronize
