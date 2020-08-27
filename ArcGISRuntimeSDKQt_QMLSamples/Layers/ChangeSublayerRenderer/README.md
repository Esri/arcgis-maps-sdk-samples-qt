# Change sublayer renderer

Apply a renderer to a sublayer.

![](screenshot.png)

## Use case

A layer showing animal populations contains sublayers for different species. A renderer could be applied which gives each sublayer a different color, so that populations of each species can be compared visually.

## How to use the sample

Wait for the map image layer to load. Press the `Apply Renderer` button to apply a unique value renderer to see different population ranges in the counties sub-layer data. Press the `Reset` button to go back to the original renderer.

## How it works

1. Create an `ArcGISMapImageLayer` from its URL.
2. After it is done loading, get its `SublayerList` with `imageLayer.mapImageSublayers`.
3. Create a `ClassBreaksRenderer` with a collection of `ClassBreak`s for different population ranges.
4. Set the renderer of the sublayer with `sublayer.renderer`.

## Relevant API

* ArcGISMapImageLayer
* ArcGISMapImageSubLayer
* ClassBreaksRenderer
* ClassBreaksRenderer.classBreak

## About the data

This application displays census data from an ArcGIS Server map service. It contains various population statistics, including total population for each county in 2007.

## Additional information

The service hosting the layer must support dynamic layers to be able to change the rendering of sublayers.

## Tags

class breaks, dynamic layer, dynamic rendering, renderer, sublayer, symbology, visualization
