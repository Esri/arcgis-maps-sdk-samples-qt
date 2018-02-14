# Generate offline map

This sample demonstrates how to take a web map offline.

![](screenshot.png)

## How it works
This sample uses the `OfflineMapTask` to take a web map offline. The sample creates a `PortalItem` object using a web map’s ID. This portal item is also used to initialize an `OfflineMapTask` object. When the button is clicked, the sample requests the default parameters for the task with the selected extent by calling `OfflineMapTask::createDefaultGenerateOfflineMapParameters`. Once the parameters are retrieved, they, along with a URL to the download path for the offline map, are then used to create a `GenerateOfflineMapJob` object from the offline map task. This job is then started and on successful completion the offline map is added to the map view. To provide feedback to the user, the progress property of `GenerateOfflineMapJob` is displayed in a window.

As the web map that is being taken offline contains an Esri basemap, this sample requires that you sign in with an ArcGIS Online organizational account.

## Features
- Map
- OfflineMapTask
- GenerateOfflineMapParameters
- GenerateOfflineMapJob
- GenerateOfflineMapResult
- MapView
- EnveloperBuilder
- AuthenticationView
- AuthenticationManager
- PortalItem
- Portal
- GeometryEngine
