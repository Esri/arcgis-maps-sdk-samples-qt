#Animate 3D Symbols
Demonstrates how to animate a graphic's position and rotation and follow it with a camera. Also shows how to combine a SceneView and MapView in an MVC application with property binding.

##How to use the sample

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

##How it works
To animate a `Graphic` by updating it's `Geometry`, heading, pitch, and roll:

1. Create a `GraphicsOverlay` and attach it to the `SceneQuickView`.
2. Create a `ModelSceneSymbol`.
3. Create a `Graphic(Geometry, Symbol)`.
 - set geometry to a point where graphic will be located in scene view
 - set symbol to the one we made above
4. Add Attributes to graphic.
 - Get attributes from graphic, `graphic.attributes()`.
 - Add heading, pitch, and roll attribute, `attributes.insertAttribute("HEADING", myHeading)`;
5. Create a SimpleRenderer to access and set it's expression properties.
 - access properties with `renderer->sceneProperties()`
 - set heading, pitch, and roll expression properties, `headingExpression("HEADING")`.
6. Add graphic to the graphics overlay.
7. Add a renderer to graphics overlay
8. Update graphic's location, `graphic.geometry = geometry`.
9. Update graphics's heading, pitch, and roll, `graphic.attributes.replaceAttribute("HEADING", myHeading)`.
10. Set the sceneViews's camera to be looking at the symbol with `sceneView.setViewpointCamera`

##Features
 - Map
 - Scene
 - Camera
 - Graphic
 - GraphicsOverlay
 - SurfacePlacement
 - MapView
 - ModelSceneSymbol
 - Point
 - Polyline
 - Renderer
 - SceneProperties
 - SceneView
 - Viewpoint
 
 ##Offline Data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[Model Marker Symbol Data](https://www.arcgis.com/home/item.html?id=810a20c8be914170948d4c2d2146b4c5)| `<userhome>`/ArcGIS/Runtime/Data/3D/SkyCrane/SkyCrane.lwo |

