# Geodesic operations

This sample demonstrates how to perform geodesic operations on geometries using the GeometryEngine. Geodesic means that calculations will take into account the curvature of the Earth, as opposed to being a planar calculation based on a 2D Cartesian plane.

A common example of utilizing geodesic operations is in visualizing flight paths from one city to another - the lines are not simply a straight line between two points, but rather a curved line. Additionally, the distance between the two locations must be calculated as geodesic distances to get an accurate account of real world length.

![](screenshot.png)

## How to use the sample
1. Click anywhere on the map. A line graphic will display the geodesic line between the two points. In addition, text that indicates the geodesic distance between the two points will be updated.
2. Click elsewhere and a new line will be created.

## How it works
1. A `Point` is created in New York City and displayed as a `Graphic`.
2. When a click occurs on the `MapView`, a new `Point` is obtained from the mouse click. This `Point` is added as a `Graphic`.
3. A `Polyline` is created with the two `Point` objects.
4. `GeometryEngine::densifyGeodetic` is executed by passing in the `Polyline` object, and the returned `Geometry` is added as a `Graphic`.
5. `GeometryEngine::lengthGeodetic` is executed by passing in the two `Point` objects, and the returned number is displayed on the screen.

## Relevant API
 - GeometryEngine::densifyGeodetic
 - GeometryEngine::lengthGeodetic

## Tags
GeometryEngine, densify, distance, geodetic, geodesic

