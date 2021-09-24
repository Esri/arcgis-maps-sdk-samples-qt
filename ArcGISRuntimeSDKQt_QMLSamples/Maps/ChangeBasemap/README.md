# Change basemap

Change a map's basemap. A basemap is beneath all layers on a `Map` and is used to provide visual reference for the operational layers.

![](screenshot.png)

## Use case

Basemaps should be selected contextually, for example, in maritime applications, it would be more appropriate to use a basemap of the world's oceans as opposed to a basemap of the world's streets.

## How to use the sample

Press the **Change Basemap** button to display the Basemap Gallery. When the gallery appears, select a basemap to be displayed.

## How it works

1. Create a `Map`.
2. Set the map to the `MapView`.
3. Create a `BasemapGallery` using the toolkit.
4. Assign the map to the `geoModel` property of the `BasemapGallery`. 

## Relevant API

* Enums.BasemapStyle
* Map
* MapView

## Tags

basemap, map
