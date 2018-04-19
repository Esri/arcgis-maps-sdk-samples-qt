# Cut geometry

This sample demonstrates how to cut a geometry with a polyline using the GeometryEngine. For example, you may want to subdivide a large parcel polygon into several smaller parcels.

![](screenshot.png)

## How to use the sample
Press the `cut` button, and the solid blue polygon will be cut into two polygons - a yellow and a green polygon with a diagonal pattern.

## How it works
The `GeometryEngine::cut` method is executed with the polygon as the first parameter and the cutting line as the second parameter. This returns a list of geometries. Each geometry is added as a new `Graphic` with differing fill symbols to visualize the results.

## Relevant API
 - GeometryEngine
 - GeometryEngine::cut

## Tags
geometry, cut, split
