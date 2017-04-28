# Raster function (file)

This sample demonstrates how to apply a RasterFunction to a local Raster file and display the output with a RasterLayer.

![](screenshot.png)

## How it works
A `RasterFunction` object is created by setting the path to a JSON file on disk. This particular raster function JSON file contains several different raster functions, including Blend, Pan Sharpen, Colormap to RGB, Color Ramp, Stretch, and Hillshade. The JSON file also defines 2 input raster arguments, so those raster arguments must be set once the `RasterFunction` object is created. Finally, a `Raster` is created from the `RasterFunction`, and the `Raster` is set on a `RasterLayer` for display in the `Map`.

## Features
- RasterFunction
- RasterLayer
- Raster
- Map

