# Change sublayer renderer

This sample demonstrates how to change the renderer on a map image layer sublayer.

In this example, a class breaks renderer is applied to see the different population ranges in the counties sublayer data.

![](screenshot.png)

## How to use the sample
Press the Apply Renderer button to apply the class breaks renderer to the sublayer. Press the Reset button to go back to the original renderer.

## How it works
- The `ArcGISMapImageSublayer` is obtained from the `ArcGISMapImageLayer` once the layer is loaded.
- A `ClassBreaksRenderer` is created by defining a series of `ClassBreaks`.
- When the Apply Renderer button is clicked, the ClassBreaksRenderer is set on the sublayer.

## Relevant API
- ArcGISMapImageSublayer
- ClassBreaksRenderer
- ClassBreak
- ArcGISMapImageSublayer.renderer

## Tags
Sublayer renderer, renderer, dynamic layer

