# Create terrain surface from a local raster

Set the terrain surface with elevation described by a raster file.

![](screenshot.png)

## Use case

In a scene view, the terrain surface is what the basemap, operational layers, and graphics are draped onto. For example, when viewing a scene in a mountainous region, applying a terrain surface to the scene will help in recognizing the slopes, valleys, and elevated areas.

## How to use the sample

When loaded, the sample will show a scene with a terrain surface applied. Pan and zoom to explore the scene and observe how the terrain surface allows visualizing elevation differences.

## How it works

1. Create a `Scene` and add it to a `SceneView`.
2. Create a `RasterElevationSource` with a list of local raster file paths, in this case a single .dt2 file
3. Add the raster elevation source to the scene's base surface.

## Relevant API

* RasterElevationSource
* Surface

## Offline data

Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples#use-offline-data-in-the-samples).

Link | Local Location
---------|-------|
|[MontereyElevation Raster](https://www.arcgis.com/home/item.html?id=98092369c4ae4d549bbbd45dba993ebc)| `<userhome>`/ArcGIS/Runtime/Data/raster/MontereyElevation.dt2 |

## Additional information

 Supported raster formats include:

* ASRP/USRP
* CIB1, 5, 10
* DTED0, 1, 2
* GeoTIFF
* HFA
* HRE
* IMG
* JPEG
* JPEG 2000
* NITF
* PNG
* RPF
* SRTM1, 2

## Tags

3D, Raster, Elevation, Surface
