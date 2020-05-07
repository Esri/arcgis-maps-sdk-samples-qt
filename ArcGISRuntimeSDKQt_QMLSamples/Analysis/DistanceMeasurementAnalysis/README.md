# Distance measurement analysis

This sample demonstrates measuring 3D distances between two points in a scene.

The distance measurement analysis allows you to add the same measuring experience found in ArcGIS Pro, City Engine, and the ArcGIS API for JavaScript to your app. You can set the unit system of measurement (metric or imperial) and have the units automatically switch to one appropriate for the current scale. The rendering is handled internally so they do not interfere with other analyses like viewsheds.

![](screenshot.png)

## How to use the sample

Choose a unit system for the measurement in the UI dropdown. Click any location in the scene to set the starting measuring point. Press, hold, and drag to a location to update the end location.

## How it works

To measure distances with the `LocationDistanceMeasurement` analysis:
1. Create an `AnalysisOverlay` and add it to your scene view's analysis overlay collection.
2. Create a `LocationDistanceMeasurement`, specifying the `startLocation` and `endLocation`. To start with, these locations can be the same. Add the analysis to the analysis overlay. The measuring line will be drawn between the two points.
3. The `directDistanceChanged`, `verticalDistanceChanged`, and `horizontalDistanceChanged` signals will emit when the distances change, giving access to the new values for the `directDistance`, `horizontalDistance`, and `verticalDistance`. The distance objects contain both the scalar value and unit of measurement.

## Relevant API

* AnalysisOverlay
* LocationDistanceMeasurement

## Additional information

The `LocationDistanceMeasurement` analysis only performs planar distance calculations. This may not be appropriate for large distances where the Earth's curvature needs to be taken into account.

## Tags
3D, Analysis

