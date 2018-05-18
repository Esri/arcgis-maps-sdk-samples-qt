# Open a scene (portal item)

This sample demonstrates how to open a scene from a Portal item. Just like web maps are the ArcGIS format for maps, web scenes are the ArcGIS format for scenes. These scenes can be stored in ArcGIS Online or Portal.

![](screenshot.png)

## How to use the sample
When the sample loads, a scene from ArcGIS Online will be displayed.

## How it works
- A `PortalItem` is created with an item ID.
- The `PortalItem` is used to create a `Scene`.
- The `Scene` is set on the `SceneView`.

## Relevant API
- Scene
- PortalItem

## Tags
Scenes, Web Scene, Portal
