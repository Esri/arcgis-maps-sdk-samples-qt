# Hillshade renderer

Apply a hillshade renderer to a raster.

![](screenshot.png)

## Use case

An environmental agency may track coastal erosion by comparing images of an area taken over a a longer period of time with hillshade renderers applied.

## How to use the sample

Choose and adjust the settings to update the hillshade renderer on the raster layer. The sample allows you to change the Altitude, Azimuth, and Slope Type.

## How it works
A `Raster` is constructed from the local raster file. The `Raster` is then used to construct a `RasterLayer`, which is then added to a `Map`. Finally `HillshadeRenderer` is created, several of the properties are set (azimuth, angle, and so on), and the renderer is set on the `RasterLayer`. You can modify the look of the `HillshadeRenderer` by pressing the Edit Renderer button, and changing the azimuth, angle, and slope type. Each time the apply button is clicked, a new `HillshadeRenderer` is created and set on the `RasterLayer`.
1. Create a `Raster` from a grayscale raster file.
2. Create a `RasterLayer` from the raster.
3. Create a `Basemap` from the raster layer and set it to the map.
4. Create a `HillshadeRenderer`, specifying the slope type and other properties.
5. Set the hillshade renderer to be used on the raster layer with `rasterLayer.renderer`.


## Relevant API

* Basemap
* HillshadeRenderer
* Raster
* RasterLayer


## Offline Data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples#use-offline-data-in-the-samples).

Link | Local Location
---------|-------|
|[Hillshade raster](https://www.arcgis.com/home/item.html?id=ae9739163a76437ea02482e1a807b806)| `<userhome>`/ArcGIS/Runtime/Data/raster/srtm.tiff |

## Tags

altitude, angle, azimuth, raster, slope, visualization
