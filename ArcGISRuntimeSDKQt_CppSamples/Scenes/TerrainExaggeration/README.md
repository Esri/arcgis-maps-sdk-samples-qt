# Terrain Exaggeration
This sample demonstrates how to exaggerate a scene's terrain surface.

![](screenshot.png)

## How to use the sample
Click and drag the slider at the bottom to change how exaggerated the terrain becomes.

## How it works
1. Create a scene.
2. Add a surface feature layer.
3. Surfaces have a property known as `elevationExaggeration` which allows you to change the surface exaggeration. Set this value as an attribute of the `Surface` object.
4. In this case, we have a slider object with values between 1 and 10. We've bound the surface's elevationExaggeration to the slider's value, which allows for a dynamic demonstration of the exaggeration effects.

## Relevant API
 - `Surface`
 - `setElevationExaggeration`

## Tags
Scenes, 3D, elevation, exaggeration, C++

