# Local Server map image layer

Start the Local Server and Local Map Service, create an ArcGIS Map Image Layer from the Local Map Service, and add it to a map.

![](screenshot.png)

## Use case

For executing offline geoprocessing tasks in your ArcGIS Runtime apps via an offline (local) server.

## How to use the sample

The Local Server and local map service will automatically be started and, once running, a map image layer will be created and added to the map.

## How it works

1. Create and run the Local Server.
    * `LocalServer::instance` creates the Local Server.
    * `LocalServer::start()` starts the server asynchronously.
2. Wait for server to be in the  `LocalServerStatus::Started` state.
    * `LocalServer::statusChanged()` fires whenever the running status of the Local Server changes.
3. Create and run a local service, example of running a `LocalMapService`.
    * `new LocalMapService(Url)`, creates a local map service with the given URL path to the map package (`mpk` file).
    * `LocalMapService::start()` starts the service asynchronously.
    * The service is added to the Local Server automatically.
4. Wait for map service to be in the `LocalServerStatus::Started` state.
    * `LocalMapService::statusChanged()` signal fires whenever the status of the local service has changed.
5. Create an ArcGIS map image layer from local map service.
    * Create a `ArcGISMapImageLayer(Url)` from local map service URL, `LocalMapService::url()`
	* Add the layer to the map's operational layers.
	* Connect to the map image layer's `LoadStatusChanged` signal.
	* When the layer's status is `Loaded`, set the map view's extent to the layer's full extent.

## Relevant API

* ArcGISMapImageLayer
* LocalMapService
* LocalServer
* LocalServerStatus

## Offline data

Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples#use-offline-data-in-the-samples).

Link | Local Location
---------|-------|
|[RelationshipID map package](https://www.arcgis.com/home/item.html?id=dee5d8060a6048a4b063484199a9546b)| `<userhome>`/ArcGIS/Runtime/Data/mpk/RelationshipID.mpk

## Additional information

Local Server can be downloaded for Windows and Linux platforms. Local Server is not supported on **macOS**.

## Tags

image, layer, local, offline, server
