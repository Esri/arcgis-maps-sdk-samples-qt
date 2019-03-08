# Scene Properties Expressions

This sample demonstrates how to update the orientation of a graphic using scene property rotation expressions.

![](screenshot.png)

## How to use the sample
Move the `Heading` and `Pitch` sliders to change the orientation of graphics in the graphics overlay.

## How it works
The sample creates a GraphicsOverlay and a SimpleRenderer. The SimpleRenderer has expressions for `HEADING` and `PITCH`, which are set via `simpleRenderer.setHeadingExpression("[HEADING]");`. The simple renderer is set to the graphics overlay via `graphicsOverlay->setRenderer(simpleRenderer);`. A graphic is created and added to the graphics overlay. The graphic's rotation is set via `m_graphic->attributes()->insertAttribute(HEADING, 0);` where the attribute key is the expression and the value is the rotation angle.

## Relevant API
 * SimpleRenderer
 * GraphicsOverlay
 * RendererSceneProperties

## Tags
Scenes, 3D, Expressions, Attributes, rotation, heading, pitch

