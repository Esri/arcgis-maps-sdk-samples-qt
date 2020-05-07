# Blend raster layer

Blend a hillshade with a raster by specifying the elevation data. The resulting raster looks similar to the original raster, but with some terrain shading, giving it a textured look.

![](screenshot.png)


## Use case

`BlendRenderer` can be used to apply a color ramp to a hillshade to emphasize areas of high or low elevation. A `BlendRenderer` can also be used to add a hillshade effect to aerial or satellite imagery, thereby making changes in elevation more visible.

## How to use the sample

Choose and adjust the altitude, azimuth, slope type and color ramp type settings to update the image.

## How it works

1. Create a `Raster` object from a raster file.
2. Create a `RasterLayer` object from the raster.
3. Create a `Basemap` object from the raster layer and set it to the map.
4. Create another `Raster` object for elevation from a grayscale raster file.
5. Create a `BlendRenderer` object, specifying the elevation raster, color ramp, and other properties.
    - If you specify a non-null color ramp, use the elevation raster as the base raster in addition to the elevation raster parameter. That way, the color ramp is used instead of the satellite imagery.
6. Set the blend renderer to the raster layer.

## Relevant API

* BlendRenderer
* ColorRamp
* Raster
* RasterLayer


## Offline Data

Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[Shasta.tif raster](https://www.arcgis.com/home/item.html?id=c669445e6cb4490b8306f0c170a9cbb1)| `<userhome>`/ArcGIS/Runtime/Data/raster/Shasta.tif |
|[Shasta_Elevation.tif raster](https://www.arcgis.com/home/item.html?id=b051f5c3e01048f3bf11c59b41507896)| `<userhome>`/ArcGIS/Runtime/Data/raster/Shasta_Elevation.tif |

## Tags

color ramp, elevation, hillshade, image, raster, raster layer, visualization