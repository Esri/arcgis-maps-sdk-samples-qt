# Local Server feature layer

Demonstrates how to start the Local Server and local feature service, create a feature layer from the local feature service, and add it to a map.

**Note:** Local Server is not supported on macOS.

![](screenshot.png)

## How to use the sample

The Local Server and local feature service will automatically be started and, once running, a feature layer is created and added to the map.

## How it works

To create a `FeatureLayer` from a `LocalFeatureService`:

1. Create and run the Local Server.
    * `LocalServer::instance` creates the Local Server.
    * `LocalServer::start()` starts the server asynchronously.
2. Wait for server to be in the `LocalServerStatus::STARTED` state.
    * `LocalServer::statusChanged()` fires whenever the running status of the Local Server changes.
3. Create and run a local service. For example, to run a `LocalMapService`:
    * `new LocalFeatureService(Url)` creates a local feature service with the given URL path to the map package (`mpk` file).
    * `LocalFeatureService::start()` starts the service asynchronously.
    * The service is added to the Local Server automatically.
4. Wait for feature service to be in the `LocalServerStatus::STARTED` state.
    * `LocalFeatureService::statusChanged()` signal fires whenever the status of the Local Service changes.
5. Create a feature layer from local feature service.
    * Create a `ServiceFeatureTable(Url)` from local feature service URL, `LocalFeatureService::url()`.
	* Create a `FeatureLayer` using the service feature table.
	* Add the feature layer to the map's operational layers.
	* Connect to the feature layer's `LoadStatusChanged` signal.
	* When the feature layer's status is `Loaded`, set the map view's extent to the layer's full extent.

## Features

* Feature Layer
* LocalFeatureService
* LocalServer
* LocalServerStatus

## Offline Data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[PointsOfInterest map package](https://www.arcgis.com/home/item.html?id=4e94fec734434d1288e6ebe36c3c461f)| `<userhome>`/ArcGIS/Runtime/Data/mpk/PointsofInterest.mpk |
