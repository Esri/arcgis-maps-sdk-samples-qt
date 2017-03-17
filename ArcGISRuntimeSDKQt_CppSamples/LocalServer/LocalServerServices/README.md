# Local Server Services

Demonstrates how to start and stop the Local Server and start and stop a Local Map, Feature, and Geoprocessing Service to that server.

**Note:** Local Server is not supported on MacOS

![](LocalServerServices.png)

## How to use the sample

Local Server Controls (Top Left):
*   Start Local Server -- Starts the Local Server if one is not already running.
*   Stop Local Server --  Stops the Local Server if one is running.

Local Services Controls (Top Right):
*   Combo Box -- Allows for the selection of a Local Map, Feature, or Geoprocessing Service.
*   Start Service -- Starts the Service that is selected in the combo box.
*   Stop Service --  Stops the Service that is selected in the `List of Running Services`.

Text Area (Middle):
* Displays the running status of the Local Server and any services that are added to that server.

List of Running Services (Bottom):
*   Displays any services that are currently running on the server.
*   Clicking on the service -- Opens browser to the service.

## How it works

To start a `LocalServer` and start a `LocalService` to it:

1. Create and run the local server.
    * `LocalServer::instance` creates the local server
    * `LocalServer::start()` starts the server asynchronously
2. Wait for server to be in the  `LocalServerStatus::STARTED` state.
    * `LocalServer::statusChanged()` fires whenever the running status of the local server has changed.
3. Create and run a local service, example of running a `LocalMapService`.
    * `new LocalMapService(Url)`, creates a local map service with the given url path to mpk file
    * `Service.startAsync()`, starts the service asynchronously
    * service will be added to the local server automatically


To stop a `LocalServer` and stop any `LocalServices` that are added to it:


1. Get any services that are currently running on the local server, `LocalServer::services()`.
2. Loop through all services and stop the selected service (from the dropdown) if started.
    * check service is started, `LocalService::status()` equals `LocalServerStatus.STARTED`
    * `LocalService::stop()`, stops the service asynchronously
3. Wait for all services to be in the `LocalServerStatus.STOPPED` state.
    * `LocalService::statusChanged()` fires whenever the running status of the local service has changed.
4. `Stop the local server, `LocalServer::stop()`.


## Features


* LocalFeatureService
* LocalGeoprocessingService
* LocalMapService
* LocalServer
* StatusChangedEvent
* LocalServerStatus
* LocalService
