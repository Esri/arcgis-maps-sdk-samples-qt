# Display draw status

Get the draw status of your map view or scene view to know when all layers in the map or scene have finished drawing.

![](screenshot.png)

## Use case

You may want to display a loading indicator while layers are loading.

## How it works

1. Create a `MapView` and connect to the `MapQuickView::drawStatusChanged` signal.
2. Use `drawStatus` on the to determine draw status.

## Relevant API

* Map
* drawStatus
* MapQuickView::drawStatusChanged
* MapView

## Tags

draw, loading, map, render
