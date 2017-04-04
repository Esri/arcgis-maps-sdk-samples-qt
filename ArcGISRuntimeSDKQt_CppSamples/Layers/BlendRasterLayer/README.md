# RGB Renderer

This sample demonstrates how to use blend renderer on a raster layer.
You can get a hillshade blended with either a colored raster or color ramp.

![](screenshot.png)

## How it works
- Tap on the "Edit Renderer" button in the toolbar to change the settings for the blend renderer.
- The sample allows you to change the altitude, azimuth, alope type and color ramp type.
- You can tap on the "Render" button to update the raster.
- If you use "none" as the color ramp type, the colored raster is blended with the hillshade output.
- For all the other types a color ramp is used.

## Features
- `Map`
- `MapQuickView`
- `Raster`
- `RasterLayer`
- `BlendRenderer`
