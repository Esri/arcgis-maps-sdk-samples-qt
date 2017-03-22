# Map loaded

This sample demonstrates how to determine the map's load status.

![](screenshot.png)

## How it works

The load status is obtained from the `loadStatus` property. The map is considered loaded when any of the following are true: the map has a valid spatial reference, the map has an initial viewpoint, or one of the map's predefined layers has been created. A signal handler is set up on the map to handle the `loadStatusChanged` signal by updating the text at the bottom of the display with the new load status.

## Features
- MapView
- Map
- Basemap

