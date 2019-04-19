# Stretch renderer

This sample demonstrates how to use stretch renderer on a raster layer.

![](screenshot.png)

## How to use the sample

  - Tap on the “Edit renderer” button to change the settings for the
    stretch renderer.
  - The sample allows you to change the stretch type and the parameters
    for each type. You can tap on the Render button to update the
    raster.
  - Experiment with settings for the various types for stretch
    parameters. For example, setting the renderer to use stretch
    parameters:
  - Min Max with a min value of 50 and a max value of 200 will stretch
    between these pixel values. A higher min value will remove more of
    the lighter pixels values whilst a lower max will remove more of the
    darker.
  - Percent Clip with a min value of 2 and a max value of 98 will
    stretch from 2% to 98% of the pixel values histogram. A lower min
    and higher max percentage will render using more of the original
    raster histogram.
  - Standard Deviation with a factor of 2.0 will stretch 2 standard
    deviations from the mean. A higher factor (further from the mean)
    will render using more of the original raster histogram.

## How it works

To apply a `StretchRenderer` to a `RasterLayer`:

  - Create a `Raster` from a raster file.
  - Create a `RasterLayer` from the raster.
  - Create a `Basemap` from the raster layer and set it to the map.
  - Create a `StretchRenderer`, specifying the `StretchParameters` and
    other properties.
  - Set the renderer on the raster layer with `rasterLayer.renderer =
    renderer`.

## Offline Data

Read more about how to set up the sample’s offline data
[here](http://links.esri.com/ArcGISRuntimeQtSamples).

| Link                                                                                           | Local Location                                     |
| ---------------------------------------------------------------------------------------------- | -------------------------------------------------- |
| [Shasta.tif raster](https://www.arcgis.com/home/item.html?id=c669445e6cb4490b8306f0c170a9cbb1) | `<userhome>`/ArcGIS/Runtime/Data/raster/Shasta.tif |
