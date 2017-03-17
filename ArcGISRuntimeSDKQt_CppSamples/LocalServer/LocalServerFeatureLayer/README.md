# Local Server Feature Layer

Demonstrates how to start a Local Server and Local Feature Service, then create a Feature Layer from the Local Feature Service and add it to a map.

**Note:** Local Server is not supported on MacOS

![](LocalServerFeatureLayer.png)

## How to use the sample

A Local Server and Local Feature Service will automatically be started and once running a Feature Layer will be created and added to the map.

## How it works

To create a `FeatureLayer` from a `LocalFeatureService`:

1. Create and run the local server.
    * `LocalServer::instance` creates the local server
    * `LocalServer::start()` starts the server asynchronously
2. Wait for server to be in the  `LocalServerStatus::STARTED` state.
    * `LocalServer::statusChanged()` fires whenever the running status of the local server has changed.
3. Create and run a local service, example of running a `LocalMapService`.
    * `new LocalFeatureService(Url)`, creates a local feature service with the given url path to mpk file
    * `LocalFeatureService::start()`, starts the service asynchronously
    * service will be added to the local server automatically
4. Wait for feature service to be in the LocalServerStatus::STARTED state.
    * `LocalFeatureService::statusChanged()` signal fires whenever the status of the local service has changed.
5. Create a feature layer from local feature service.
    * create a `ServiceFeatureTable(Url)` from local feature service url, `LocalFeatureService::url()`
	* create a `FeatureLayer` using the service feature table
	* add the feature layer to the map's operational layer
	* connect to the feature layer's `LoadStatusChanged` signal
	* once the status is `Loaded`, set the mapview's extent to the layer's full extent.

## Features
* Feature Layer
* LocalFeatureService
* LocalServer
* LocalServerStatus
