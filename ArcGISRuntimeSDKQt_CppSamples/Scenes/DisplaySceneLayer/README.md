# Display a scene layer

Add a scene layer to a scene.

![](screenshot.png)

## Use case

Each scene layer added to a scene can assist in performing helpful visual analysis. For example, if presenting the results of a shadow analysis of a major metropolitan downtown area in 3D, adding a scene layer of 3D buildings to the scene that could be toggled on/off would help to better contextualize the source of the shadows.

## How to use the sample

When launched, this sample displays a scene service with an `ArcGISSceneLayer`. Pan and zoom to explore the scene.

## How it works

1. Create an `Scene` and set its `Basemap` with `Scene::setBasemap()`.
2. Create a `Surface` and add an elevation source to it: `surface::elevationSources()::append(arcGISTiledElevationSource)`.
3. Add the created surface to the scene: `Scene::setBaseSurface(surface)`.
4. Create a `SceneView` and set the scene to the view with `SceneView::setArcGISScene(scene)`.
5. Create an `ArcGISSceneLayer` using a data source URL: `new ArcGISSceneLayer(URL)`.
6. Add the new scene layer to the scene as an operational layer with `ArcGISScene::operationalLayers()::append(sceneLayer)`.

## About the data

The scene launches with a northward view of the city of Brest, France. A 3D scene layer representing buildings (some textured) is used as an example.

## Relevant API

* Scene
* ArcGISSceneLayer
* ArcGISTiledElevationSource
* SceneView
* Surface

## Tags

3D, layer, scene
