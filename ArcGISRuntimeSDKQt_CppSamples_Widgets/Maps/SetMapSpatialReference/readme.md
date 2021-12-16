# Set map spatial reference

This sample demonstrates how to set the initial spatial reference of a map so that all layers that support reprojection are projected into the map’s spatial reference.

![](screenshot.png)

## How it works

The basemap contains an `ArcGISMapImageLayer`, which supports reprojection, and has a default spatial reference of `GCS_WGS_1984 (WKID: 4326)`.
When the basemap is added to the map, the `ArcGISMapImageLayer` is automatically reprojected into the map’s spatial reference.

## Features

- MapView
- Map
- Basemap
- SpatialReference

