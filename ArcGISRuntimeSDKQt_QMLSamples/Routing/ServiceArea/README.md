# Service area

Find the service area within a network from a given point.

![](screenshot.png)

## Use case

A service area shows locations that can be reached from a facility based off a certain impedance, such as travel time or distance. Barriers can increase impedance by either adding to the time it takes to pass through the barrier or by altogether preventing passage.

You might calculate the region around a hospital in which ambulances can service in 30 min or less.

## How to use sample

* In order to find any services areas at least one `ServiceAreaFaciltiy` needs to be added.
* To add a facility, select "Facility" from the combo-box then click anywhere on the `MapView`.
* To add a barrier, select "Barrier" from the combo-box and click multiple locations on the `MapView` to draw a barrier.
* To start a new line in a distinct location, click "new barrier"
* To show service areas around facilities that were added, click the "Solve" button.
* The "Reset" button, clears all graphics and resets the `ServiceAreaTask`.

## How it works

1. Create a `ServiceAreaTask` from an online service.
2. Create default `ServiceAreaParameters` from the service area task.
3. Set the parameters to return polygons (true) to return all service areas.
4. Add a `ServiceAreaFacility` to the parameters.
5. Get the `ServiceAreaResult` by solving the service area task using the parameters.
6. Get any `ServiceAreaPolygons` that were returned using `ServiceAreaResult.resultPolygons`
7. Display service areas to `MapView` by creating graphics for their geometry and adding to a graphics overlay.

## Relevant API

* PolylineBarrier
* ServiceAreaFacility
* ServiceAreaParameters
* ServiceAreaPolygon
* ServiceAreaResult
* ServiceAreaTask

## Tags

barriers, facilities, impedance, logistics, routing
