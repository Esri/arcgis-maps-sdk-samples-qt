#Map rotation

![](screenshot.png) 

##

This sample demonstrates how to set the initial spatial reference of a map and all its layers would be projected accordingly.

## How it works

The map in the sample is initialized with `esriSRProjCS_World_Bonne (WKID: 54024)` spatial reference. 

Hence the Basemap that is added, with default spatial reference as `GCS_WGS_1984 (WKID: 4326)`, gets re-projected to map's spatial reference.

## Features

- MapView
- Map
- Basemap
- SpatialReference

