# Filter features in scene

Filter 3D scene features out of a given geometry with a polygon filter.

![](screenshot.png)

## Use case

You can selectively show or hide features within a scene view using `setFeaturesVisible` to filter specific features or set a `SceneLayerPolygonFilter` to filter features given a polygon. Through these methods, you can directly control what users see within a specific scene view to give a more focused or cleaner user experience.

## How to use the sample

The sample initializes showing the 3D buildings OpenStreetMap layer. Click the "Load detailed buildings" button to load an additional scene layer that contains more detailed buildings. Notice how the two scene layers overlap and clip into each other. Click the "Filter OSM buildings in extent" button, to set a `SceneLayerPolygonFilter` and filter out the OpenStreetMap buildings within the extent of the detailed buildings scene. Notice how the OSM buildings within and intersecting the extent of the detailed buildings layer are hidden. Click the "Reset scene" button to hide the detailed buildings scene layer and clear the OSM buildings filter.

## How it works

1. Construct a basemap for the scene using an OpenStreetMap Topographic vector tile layer and the OpenStreetMap 3D Buildings layer as baselayers.
2. Create a surface for the scene and set the World Elevation 3D as an elevation source.
3. Add the 3D San Fransisco Buildings Scene Layer to the scene's operational layers.
4. Construct a `SceneLayerPolygonFilter` with the extent of the San Fransisco Buildings Scene Layer and a `Disjoint` `SceneLayerPolygonFilterSpatialRelationship` to hide all features within the extent.
5. Set the `SceneLayerPolygonFilter` on the OSM Buildings layer to hide all OSM buildings within the extent of the San Fransisco Buildings layer.

## Relevant API

* ArcGISSceneLayer
* SceneLayerPolygonFilter
* SceneLayerPolygonFilterSpatialRelationship

## About the data

This sample uses the [OpenStreetMap 3D Buildings](https://www.arcgis.com/home/item.html?id=ca0470dbbddb4db28bad74ed39949e25) which provides generic 3D outlines of buildings throughout the world. It is based on the OSM Daylight map distribution and is hosted by Esri. It uses the [San Fransisco 3D Buildings](https://www.arcgis.com/home/item.html?id=d3344ba99c3f4efaa909ccfbcc052ed5) scene layer which provides detailed 3D models of buildings in San Fransisco, California, USA.


## Tags

3D, buildings, disjoint, exclude, extent, filter, hide, OSM, polygon
