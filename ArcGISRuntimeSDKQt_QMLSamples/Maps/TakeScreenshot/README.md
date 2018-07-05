# Take screenshot

This sample demonstrates how to take a screenshot of the MapView. This could be useful for sending an image of your map to a coworker, or it could be used for testing and debugging your applications. This same workflow works with the 3D SceneView as well.

![](screenshot.png)

## How to use the sample
Open the sample and navigate to a desired location on the map. Press the `Take screenshot` button, and a screenshot image will display over the map. You can close the image by pressing the close button. Continue navigating the map and generating new screenshots.

## How it works
- The `exportImage` function is executed on the `MapView`.
- Once the asynchronous task completes, a QML Image uses the image URL to render an Image on the screen.

## Relevant API
 - GeoView::exportImage

## Tags
screenshot, screen capture, export

