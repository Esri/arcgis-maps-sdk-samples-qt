# Filter features in scene

Filter 3D scene features out of a given geometry with a polygon filter.

![](screenshot.png)

## Use case

You can selectively show or hide features within a scene view using `setFeaturesVisible` to filter specific features or set a `SceneLayerPolygonFilter` to filter features given a polygon. Through these methods, you can directly control what users see within a specific scene view to give a more focused or cleaner user experience.

## How to use the sample

The sample initializes showing the 3D buildings OpenStreetMap layer. Click the "Load detailed buildings" button to load an additional scene layer that contains more detailed buildings. Notice how the two scene layers overlap and clip into each other. Click the "Filter OSM buildings in extent" button, to set a `SceneLayerPolygonFilter` and filter out the OpenStreetMap buildings within the extent of the detailed buildings scene. Notice how the OSM buildings within and intersecting the extent of the detailed buildings layer are hidden. Click the "Reset scene" button to hide the detailed buildings scene layer and clear the OSM buildings filter.

## How it works

1. Construct a `Basemap` for the scene using an OpenStreetMap Topographic `VectorTileLayer` and the OpenStreetMap 3D Buildings `ArcGISSceneLayer` as baselayers.
2. Set the OSM buildings layer's `polygonFilter` property to a `SceneLayerPolygonFilter` with an empty list of polygons and a `SceneLayerPolygonFilterSpatialRelationshipDisjoint` enum to hide all features within the extent.
3. Create a `Surface` for the scene and set the World Elevation 3D as an elevation source.
4. Add the 3D San Fransisco Buildings `ArcGISSceneLayer` to the scene's operational layers.
5. When the detailed buildings layer finishes loading, build a polygon from its extent and set that polygon to the OSM building's polygon filter to hide all OSM buildings within the extent.

## Relevant API

* ArcGISSceneLayer
* SceneLayerPolygonFilter
* SceneLayerPolygonFilterSpatialRelationship

## About the data

This sample uses the [OpenStreetMap 3D Buildings](https://www.arcgis.com/home/item.html?id=ca0470dbbddb4db28bad74ed39949e25) which provides generic 3D outlines of buildings throughout the world. It is based on the OSM Daylight map distribution and is hosted by Esri. It uses the [San Fransisco 3D Buildings](https://www.arcgis.com/home/item.html?id=d3344ba99c3f4efaa909ccfbcc052ed5) scene layer which provides detailed 3D models of buildings in San Fransisco, California, USA.

## Additional information

This sample uses `SceneLayerPolygonFilterSpatialRelationship::Disjoint` to hide all features within the extent of the given geometry. You can alternatively use `SceneLayerPolygonFilterSpatialRelationship::Contains` to only show features within the extent of the geometry. 

You can also show or hide features in a scene layer using `ArcGISSceneLayer::setFeatureVisible` or `setFeaturesVisible` and pass in a feature or list of features and a boolean value to set their visibility.

## Tags

3D, buildings, disjoint, exclude, extent, filter, hide, OSM, polygon
