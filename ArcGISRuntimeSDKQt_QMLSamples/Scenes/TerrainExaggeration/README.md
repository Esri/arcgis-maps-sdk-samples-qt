# Terrain Exaggeration

This sample demonstrates how to exaggerate a scene's terrain surface.

![](screenshot.png)

## How to use the sample
Click and drag the slider at the top to change how exaggerated the terrain becomes.

## How it works
1. Create a scene.
2. Create and add a surface with an ArcGISTiledElevationService.
3. Create a slider object.
4. Bind the surface's `elevationExaggeration` property to the slider's value.

## Relevant API
 - `Surface`
 - `Surface.elevationExaggeration`

## Tags
Scenes, Elevation, 3D, Exaggeration

