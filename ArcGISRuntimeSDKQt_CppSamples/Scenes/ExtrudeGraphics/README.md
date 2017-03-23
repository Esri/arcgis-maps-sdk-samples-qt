# Extrude Graphics

This sample demonstrates how to render graphics extruded in the Z direction.

![](screenshot.png)

## How it works
To extrude graphics according to a property:

1. Create a ```GraphicsOverlay``` and ```SimpleRenderer```.
2. Get the renderer's ```SceneProperties``` using ```renderer->sceneProperties()```.
3. Set the extrusion mode for the renderer with ```props.setExtrusionMode(ExtrusionMode::BaseHeight)```.
4. Specify the attribute name of the graphic that the extrusion mode will use: ```props.setExtrusionExpression("[height]")```.
5. Set the renderer on the graphics overlay using ```graphicsOverlay->setRenderer(renderer)```.
6. Create graphics with the attribute set: ```graphic->attributes()->insertAttribute("height", z)```.

## Features
- ArcGISScene
- Graphic
- GraphicsOverlay
- Renderer
- Renderer->SceneProperties
