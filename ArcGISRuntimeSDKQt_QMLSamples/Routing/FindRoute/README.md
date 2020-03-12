# Find a route

Display directions for a route between two points.

![](screenshot.png)

## Use case

Find routes with driving directions between any number of locations. You might use the ArcGIS platform to create a custom network for routing on a private roads.

## How to use the sample

For simplicity, the sample comes loaded with a start and end stop. You can click on the Find Route to display a route between these stops. Once the route is generated, turn-by-turn directions are shown in a list.

## How it works

1. Create a `RouteTask` using a URL to an online route service.
2. Generate default `RouteParameters` using `routeTask.createDefaultParameters()`.
3. Set `returnStops` and `returnDirections` on the parameters to true.
4. Add `Stop`s to the parameters `stops` collection for each destination.
5. Solve the route using `routeTask.solveRoute(routeParameters)` to get a `RouteResult`.
6. Iterate through the result's `Route`s. To display the route, create a graphic using the geometry from `route.routeGeometry()`. To display directions, use `route.directionManeuvers()` and apply the list model to the UI.

## Relevant API

*   DirectionManeuver
*   Route
*   RouteParameters
*   RouteResult
*   RouteTask
*   Stop

## Tags

directions, driving, navigation, network, network analysis, route, routing, shortest path, turn-by-turn
