# Local Server services

Demonstrates how to start and stop the Local Server and start and stop a local map, feature, and geoprocessing gervice running on the Local Server.

**Note:** Local Server is not supported on macOS.

![](screenshot.png)

## How to use the sample

Click `Start Local Server` to start the Local Server. Click `Stop Local Server` to stop the Local Server. The `Map Service` combo box lets you to pick a local service that is available. After browsing for the desired file, click `Start Service` to start the selected service. When the running service's Url appears, select it and click `Open Url`. To stop this running service, click `Stop Service`.

## How it works

To start a `LocalServer` and start a `LocalService` to it:

1. Create and run the local server.
    * `LocalServer::instance` creates the local server.
    * `LocalServer::start()` starts the server asynchronously.
2. Wait for server to be in the  `LocalServerStatus::STARTED` state.
    * `LocalServer::statusChanged()` fires whenever the running status of the Local Server changes.
3. Create and run a local service, example of running a `LocalMapService`.
    * `new LocalMapService(Url)` creates a local map service with the given URL path to map package (`mpk` file).
    * `LocalMapService::start()` starts the service asynchronously
    * The service is added to the Local Server automatically

To stop a `LocalServer` and stop any `LocalServices` that are added to it:

1. Get any services that are currently running on the local server, `LocalServer::services()`.
2. Loop through all services and stop the selected service (from the dropdown) if it is started.
    * To check whether service is started, `LocalService::status()` equals `LocalServerStatus::STARTED`
    * `LocalService::stop()`, stops the service asynchronously
3. Wait for all services to be in the `LocalServerStatus::STOPPED` state.
    * `LocalService::statusChanged()` fires whenever the running status of the local service has changed.
4. `Stop the local server, `LocalServer::stop()`.

## Features

* LocalFeatureService
* LocalGeoprocessingService
* LocalMapService
* LocalServer
* LocalServerStatus
* LocalService


## Offline Data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[PointsOfInterest map package](https://www.arcgis.com/home/item.html?id=4e94fec734434d1288e6ebe36c3c461f)| `<userhome>`/ArcGIS/Runtime/Data/mpk/PointsofInterest.mpk |
|[MessageInABottle geoprocessing package](https://www.arcgis.com/home/item.html?id=a0ef1f20344f43ad8837f0e0d8406d03)| `<userhome>`/ArcGIS/Runtime/Data/mpk/MessageInABottle.gpk |
