# Take screenshot

Take a screenshot of the map.

![](screenshot.png)

## Use case

GIS users may want to export a screenshot of a map to enable sharing as an image or printing.

## How to use the sample

Pan and zoom to find an interesting location. Press `Take screenshot`, and a screenshot image will display over the map. Note that there may be a small delay if the map is still rendering when you push the button.

## How it works

1. The `exportImage` function is executed on the `MapView`.
2. Once the asynchronous task completes, a QML Image uses the image URL to render an Image on the screen.

## Relevant API

* GeoView.exportImage

## Tags

capture, export, image, print, screen capture, screenshot, share, shot
