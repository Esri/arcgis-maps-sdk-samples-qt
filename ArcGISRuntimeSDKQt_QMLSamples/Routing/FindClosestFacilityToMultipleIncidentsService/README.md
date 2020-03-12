# Find closest facility to multiple incidents (service)

Find routes from several locations to the respective closest facility.

![](screenshot.png)

## Use case

Quickly and accurately determining the most efficient route between a location and a facility is a frequently encountered task. For example, a city's fire department may need to know which fire stations in the vicinity offer the quickest routes to multiple fires. Solving for the closest fire station to the fire's location using an impedance of "travel time" would provide this information.

## How to use the sample

Click 'Solve Routes' to solve and display the route from each incident (fire) to the nearest facility (fire station). Click 'Reset' to clear the results.

## How it works

1. Create a `ClosestFacilityTask` using a URL from an online service.
2. Get the default set of `ClosestFacilityParameters` from the task: `closestFacilityTask.createDefaultParameters()`.
3. Create `ServiceFeatureTable`s for both facilities incidents.
4. Add all facilities to the task parameters: `closestFacilityParameters.setFacilitiesWithFeatureTable(facilitiesFeatureTable, parameters)`.
5. Add all incidents to the task parameters: `closestFacilityParameters.setIncidentsWithFeatureTable(incidentsFeatureTable, parameters)`.
6. Get `ClosestFacilityResult` by solving the task with the provided parameters: `closestFacilityTask.solveClosestFacility(closestFacilityParameters)`.
7. Find the closest facility for each incident by iterating over the list of `Incident`s.
8. Display the route as a `Graphic` using the `closestFacilityRoute.routeGeometry()`.

## Relevant API

* ClosestFacilityParameters
* ClosestFacilityResult
* ClosestFacilityRoute
* ClosestFacilityTask
* Facility
* Graphic
* GraphicsOverlay
* Incident

## Tags

incident, network analysis, route, search
