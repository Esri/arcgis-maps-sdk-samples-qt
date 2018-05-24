# Spatial relationships

This sample demonstrates how to determine the spatial relationships between two geometries.

For example, you may want to determine if geometry A intersects, touches, or is within geometry B.

![](screenshot.png)

## How to use the sample
Tap on one of the graphics to select it. The UI will list the relationships the selected graphic has to the other graphic geometries.

## How it works
- Get the geometry from two different graphics. In this example the geometry of the selected graphic is compared to the geometry of each graphic not selected.
- Use the methods in `GeometryEngine` to check the relationship between the geometries, e.g. contains, disjoint, intersects, touches, etc. If the method returns true, the relationship exists.

## Relevant API
- GeometryEngine::intersects
- GeometryEngine::touches
- GeometryEngine::disjoint
- GeometryEngine::crosses
- GeometryEngine::within
- GeometryEngine::overlaps
- GeometryEngine::contains

## Tags
Geometry Engine, Spatial Analysis, Spatial Relationships

