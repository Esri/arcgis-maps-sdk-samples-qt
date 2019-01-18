# Line of sight (geoelement)

Show a line of sight between two moving objects.

## How to use the sample

To determine if an observer can see a target, you can show a line of sight between them.
The line will be green until the line of sight is obstructed, in which case it will turn red.
By using the GeoElement variant of the line of sight, the line will automatically update when either GeoElement moves.

![](screenshot.png)

## Use case

A line of sight between `GeoElement`s (i.e. observer and target) will not remain constant whilst one or both are on the move.

A `GeoElementLineOfSight` is therefore useful in cases where visibility between two `GeoElement`s requires monitoring over a period of time in a partially obstructed field of view
(such as buildings in a city).

# How to use the sample

A line of sight will display between a point on the Empire State Building (observer) and a taxi (target).
The taxi will drive around a block and the line of sight should automatically update.
The taxi will be highlighted when it is visible. You can change the observer height with the slider to see how it affects the target's visibility.

## How it works

1. Instantiate an `AnalysisOverlay` and add it to the `SceneView`'s analysis overlays collection.
1. Instantiate a `GeoElementLineOfSight`, passing in observer and target `GeoElement`s (features or graphics). Add the line of sight to the analysis overlay's analyses collection.
1. To get the target visibility when it changes, react to the target visibility changing on the `GeoElementLineOfSight` instance.

## Relevant API

1. AnalysisOverlay
1. GeoElementLineOfSight
1. LineOfSight::targetVisibility

## Offline data

Link | Local Location
---------|-------|
|[Taxi CAD](https://www.arcgis.com/home/item.html?id=3af5cfec0fd24dac8d88aea679027cb9)|`<userhome>/ArcGIS/Runtime/Data/3D/dolmus\_3ds/dolmus.zip`|

# Tags

Analysis, line of sight, visibility
