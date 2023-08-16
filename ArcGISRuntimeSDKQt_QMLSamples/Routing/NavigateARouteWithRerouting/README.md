# Navigate route with rerouting

Navigate between two points and dynamically recalculate an alternate route when the original route is unavailable.

![](screenshot.png)

## Use case

While traveling between destinations, field workers use navigation to get live directions based on their locations. In cases where a field worker makes a wrong turn, or if the route suggested is blocked due to a road closure, it is necessary to calculate an alternate route to the original destination.

## How to use the sample

<<<<<<< HEAD
Click 'Navigate' to simulate traveling and to receive directions from a preset starting point to a preset destination. Observe how the route is recalculated when the simulation does not follow the suggested route. Check 'Voice Directions' to activate announcing maneuvers. Click 'Reset' to start the simulation from the beginning.
=======
Click 'Navigate' to simulate traveling and to receive directions from a preset starting point to a preset destination. Observe how the route is recalculated when the simulation does not follow the suggested route.
>>>>>>> v.next

## How it works

1. Create a `RouteTask` using a local geodatabase.
2. Generate default `RouteParameters` using `createDefaultParameters`.
3. Set `returnStops` and `returnDirections` on the parameters to true.
4. Add `Stop`s to the parameters `stops` collection for each destination.
5. Solve the route using to get a `RouteResult`.
6. Create a `RouteTracker` using the route result, and the index of the desired route to take.
7. Enable rerouting in the route tracker. Use `ReroutingStrategy::toNextWaypoint` to specify that in the case of a reroute the new route goes from present location to next waypoint or stop.
8. Track the location of the device and update the route tracking status.
9. Add a listener to capture and get the `TrackingStatus` and use it to display updated route information. Tracking status includes a variety of information on the route progress, such as the remaining distance, remaining geometry or traversed geometry (represented by a `Polyline`), or the remaining time (`Double`), amongst others.
10. Add a `NewVoiceGuidanceListener` to get the `VoiceGuidance` whenever new instructions are available.
11. You can also query the tracking status for the current `DirectionManeuver` index, retrieve that maneuver from the `Route` and get its direction text to display in the GUI.
12. To establish whether the destination has been reached, get the `DestinationStatus` from the tracking status. If the destination status is `REACHED`, and the `remainingDestinationCount` is 1, you have arrived at the destination and can stop routing. If there are several destinations in your route, and the remaining destination count is greater than 1, switch the route tracker to the next destination.

## Relevant API

* DestinationStatus
* DirectionManeuver
* Location
* LocationDataSource
* ReroutingStrategy
* Route
* RouteParameters
* RouteTask
* RouteTracker
* Stop
* VoiceGuidance

## Offline data

The data used by this sample is available on [ArcGIS Online](https://www.arcgis.com/home/item.html?id=4caec8c55ea2463982f1af7d9611b8d5).

Link | Local Location
---------|-------|
|[Navigate a Route JSON Track](https://www.arcgis.com/home/item.html?id=4caec8c55ea2463982f1af7d9611b8d5)| `<userhome>`/ArcGIS/Runtime/Data/tpkx/san_diego |

## About the data

The route taken in this sample goes from the San Diego Convention Center, site of the annual Esri User Conference, to the Fleet Science Center, San Diego.

## Additional information

The route tracker will start a rerouting calculation automatically as necessary when the device's location indicates that it is off-route. The route tracker also validates that the device is "on" the transportation network, if it is not (e.g. in a parking lot) rerouting will not occur until the device location indicates that it is back "on" the transportation network.

## Tags

directions, maneuver, navigation, route, turn-by-turn, voice
