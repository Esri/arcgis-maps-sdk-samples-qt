# Take screenshot

Take a screenshot of the map.

![](screenshot.png)

## Use case

GIS users may want to export a screenshot of a map to enable sharing as an image or printing.

## How to use the sample

Pan and zoom to find an interesting location. Press `Take screenshot`, and a screenshot image will display over the map. Note that there may be a small delay if the map is still rendering when you push the button.

## How it works

1. The `exportImage` function is executed on the `MapView`.
2. Once the asynchronous task completes, a `QImage` is returned.
3. QML cannot directly render a `QImage`. Instead, a QML Image Provider must be created for generating an in-memory URL to the returned `QImage`. This is done with the following steps:
  * Subclass `QQuickImageProvider`.
  * Reimplement the `requestImage` function.
  * Add an image provider to the `QQmlEngine` instance using `QQmlEngine::addImageProvider`.
  * Create a `Q_PROPERTY` that returns the image URL from the Image Provider.
  * Bind the QML Image to the image URL property. When this property changes, it will automatically trigger the `requestImage` function and return the URL to the image.

## Relevant API

* GeoView::exportImage

## Tags

capture, export, image, print, screen capture, screenshot, share, shot
