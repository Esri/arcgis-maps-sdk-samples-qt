# Animate 3D symbols
Demonstrates how to animate a graphic's position and rotation and follow it with a camera. Also shows how to combine a SceneQuickView and MapQuickView in an MVC application with property binding.

## How to use the sample

Animation Controls (Top Left Corner):
 - Select a mission -- selects a location with a route for the helicopter to fly
 - Mission progress -- shows how far along the route the helicopter is. Slide to change keyframe in animation
 - Play -- toggles playing and stopping the animation
 - Toggle -- toggles the camera's follow mode and free cam mode
 
Camera Controls (Top Right Corner):
 - Camare zoom -- distance between camera and helicopter
 - Camera angle -- viewing angle between camera and helicopter
 - Flight speed -- controls speed of animation

 2D Map Controls (Bottom Left Corner):
 - Plus and Minus -- controls distance of 2D view from ground level

![](screenshot.png)

## How it works
To animate a `Graphic` by updating it's `Geometry`, heading, pitch, and roll:

1. Create a `GraphicsOverlay` and attach it to the `SceneQuickView`.
2. Create a `ModelSceneSymbol`.
3. Create a `Graphic(Geometry, Symbol)`.
 1. Set geometry to a point where graphic will be located in scene view
 2. Set symbol to the one we made above
4. Add Attributes to graphic.
 1. Get attributes from graphic, `graphic->attributes()`.
 2. Add heading, pitch, and roll attribute, `attributes->insertAttribute("heading", myHeading)`;
5. Create a SimpleRenderer to access and set it's expression properties.
 1. Access properties with `renderer->sceneProperties()`
 2. Set heading, pitch, and roll expressions, `sceneProperties->setHeadingExpression("[heading]")`.
6. Add graphic to the graphics overlay.
7. Set renderer to graphics overlay, `graphicsOverlay->setRenderer(renderer)`
8. Update graphic's location, `graphic->setGeometry(Point)`.
9. Update graphics's heading, pitch, and roll, `graphic->attributes()->replaceAttribute("heading", myHeading)`.
10. Set the sceneViews's camera to be looking at the symbol with `sceneView->setViewpointCamera( Camera(myPoint, myZoomDist, myHeading, myAngle, myRoll), myDurationSeconds)`

## Features
 - Map
 - Scebe
 - Camera
 - Graphic
 - GraphicsOverlay
 - SurfacePlacement
 - MapQuickView
 - ModelSceneSymbol
 - Point
 - Polyline
 - Renderer
 - SceneProperties
 - SceneQuickView
 - Viewpoint
 
 ## Offline Data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[Model Marker Symbol Data](https://www.arcgis.com/home/item.html?id=681d6f7694644709a7c830ec57a2d72b)| `<userhome>`/ArcGIS/Runtime/Data/3D/Bristol/Collada/Bristol.dae|
|[GrandCanyon.csv mission data](https://www.arcgis.com/home/item.html?id=290f0c571c394461a8b58b6775d0bd63)| `<userhome>`/ArcGIS/Runtime/Data/3D/Missions/GrandCanyon.csv |
|[Hawaii.csv mission data](https://www.arcgis.com/home/item.html?id=e87c154fb9c2487f999143df5b08e9b1)| `<userhome>`/ArcGIS/Runtime/Data/3D/Missions/Hawaii.csv |
|[Pyrenees.csv mission data](https://www.arcgis.com/home/item.html?id=5a9b60cee9ba41e79640a06bcdf8084d)| `<userhome>`/ArcGIS/Runtime/Data/3D/Missions/Pyrenees.csv |
|[Snowdon.csv mission data](https://www.arcgis.com/home/item.html?id=12509ffdc684437f8f2656b0129d2c13)| `<userhome>`/ArcGIS/Runtime/Data/3D/Missions/Snowdon.csv |

