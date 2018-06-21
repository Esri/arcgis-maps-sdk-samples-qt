# Terrain Exaggeration
This sample demonstrates how to exaggerate a scene's terrain surface.

![](screenshot.png)

## How to use the sample
Click and drag the slider at the bottom to change how exaggerated the terrain becomes.

## How it works
1. Create a scene and add a slider (with an ID) to the UI via QML.
3. Surfaces have a method in the C++ API known as `setElevationExaggeration` which allows you to change the surface exaggeration. In the `.cpp` file, create a new function for this called `setExaggerationElevation` that accepts a `double` argument.
4. This function first checks that m_surface exists using an `if` statement; if this check does not happen, it's possible that the function runs before the scene is even created.
5. If `m_surface` exists, then access its `setElevationExaggeration` method and pass in the `double` parameter.
6. In the `.qml` file, ensure that the main sample object at the root of the QML heirarchy has an ID for reference.
7. Under the slider's properties, create a QML property called `OnValueChanged`. This will be called each time you change the value of the slider.
8. Call `[SAMPLE ROOT ID].setElevationExaggeration([SLIDER ID].value)`;. This binds the elevationExaggeration value to that of the slider.

## Relevant API
 - `Surface`
 - `setElevationExaggeration`

## Tags
Scenes, 3D, elevation, exaggeration, C++

