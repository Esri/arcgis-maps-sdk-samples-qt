# Hillshade renderer

This sample shows how to apply a HillshadeRenderer to a RasterLayer.

![](screenshot.png)

## How it works
A `Raster` is constructed from the local raster file. The `Raster` is then used to construct a `RasterLayer`, which is then added to a `Map`. Finally `HillshadeRenderer` is created, several of the properties are set (azimuth, angle, and so on), and the renderer is set on the `RasterLayer`. You can modify the look of the `HillshadeRenderer` by pressing the Edit Renderer button, and changing the azimuth, angle, and slope type. Each time the apply button is clicked, a new `HillshadeRenderer` is created and set on the `RasterLayer`.

## Features
- HillshadeRenderer
- RasterLayer
- Raster
- Basemap
- Map
- MapView


## Offline Data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[Hillshade raster](https://www.arcgis.com/home/item.html?id=ae9739163a76437ea02482e1a807b806)| `<userhome>`/ArcGIS/Runtime/Data/raster/srtm.tiff |
