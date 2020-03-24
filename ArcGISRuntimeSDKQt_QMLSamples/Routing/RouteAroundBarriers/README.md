# Route around barriers

Find a route that reaches all stops without crossing any barriers.

![](screenshot.png)

## Use case

You can define barriers to avoid unsafe areas, for example flooded roads, when planning the most efficient route to evacuate a hurricane zone. When solving a route, barriers allow you to define portions of the road network that cannot be traversed. You could also use this functionality to plan routes when you know an area will be inaccessible due to a community activity like an organized race or a market night.

In some situations, it is further beneficial to find the most efficient route that reaches all stops, reordering them to reduce travel time. For example, a delivery service may target a number of drop-off addresses, specifically looking to avoid congested areas or closed roads, arranging the stops in the most time-effective order.

## How to use the sample

Click 'Add stop' to add stops to the route. Click 'Add barrier' to add areas that can't be crossed by the route. Select 'Find best sequence' to allow stops to be re-ordered in order to find an optimum route. Select 'Preserve first stop' to preserve the first stop. Select 'Preserve last stop' to preserve the last stop.

## How it works

1. Construct a `RouteTask` with the URL to a Network Analysis route service.
2. Get the default `RouteParameters` for the service, and create the desired `Stop`s and `PolygonBarrier`s.
4. Add the stops and barriers to the route's parameters, `routeParameters.setStops(routeStops)` and `routeParameters.setPolygonBarriers(routeBarriers)`.
5. Set the `returnStops` and `returnDirections` to `true`.
6. If the user will accept routes with the stops in any order, set `findBestSequence` to `true` to find the most optimal route.
7. If the user has a definite start point, set `preserveFirstStop` to `true`.
8. If the user has a definite final destination, set `preserveLastStop` to `true`.
9. Call `routeTask.solveRoute(routeParameters)` to get a `RouteResult`.
10. Get the first returned route by calling `solveRouteResult.routes[0]`.
11. Get the geometry from the route to display the route to the map.


## Relevant API

* DirectionManeuverList
* PolygonBarrier
* Route
* RouteParameters
* RouteResult
* RouteTask
* Stop

## Offline Data

Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[orange_symbol Png File](https://www.arcgis.com/home/item.html?id=1c95ea3b6e4843cdbd6ae354efb97f0c)| `<userhome>`/ArcGIS/Runtime/Data/symbol/orange_symbol.png |

## About the data

This sample uses an Esri-hosted sample street network for San Diego.

## Tags

barriers, best sequence, directions, maneuver, network analysis, routing, sequence, stop order, stops

