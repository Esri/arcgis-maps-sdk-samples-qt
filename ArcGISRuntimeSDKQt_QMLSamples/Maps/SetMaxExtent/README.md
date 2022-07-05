# Set max extent

Limit the view of a map to a particular area by setting a max extent to constrain the user's ability to pan or zoom away. 

![](screenshot.png)

## How to use the sample

The application loads with a map whose maximum extent has been set to the borders of Colorado. Note that you won't be able to pan far from the Colorado border or zoom out beyond the minimum scale set by the max extent. Use the toggle switch to disable the max extent to freely pan/zoom around the map.

## How it works

1. Create a `Map` object.
2. Create an `Envelope` of the Colorado extent.
3. Set the map to a `MapView` object.
4. Set the maximum extent of the map to be the Colorado envelope with `Map.maxExtent`.
5. Set `maxExtent` to a null envelope to disable the maximum extent of the map.

## Relevant API

* Envelope
* Map

## Tags

extent, limit panning, map, mapview, max extent, zoom

