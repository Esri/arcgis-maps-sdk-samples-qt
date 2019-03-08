# Create terrain surface from a local raster

Use a terrain surface with elevation described by a raster file.

The terrain surface is what the basemap, operational layers, and graphics are draped on.

![](screenshot.png)

## How it works

1. Create a `Scene` and add it to a `SceneView`.
2. Create a `RasterElevationSource` with a list of local raster file paths, in this case a single .dt2 file
3. Add this source to the scene's base surface: `Scene.BaseSurface.ElevationSources.append(rasterElevationSource)`.

## Additional information

Supported raster formats include:
- ASRP/USRP
- CIB1, 5, 10
- DTED0, 1, 2
- GeoTIFF
- HFA
- HRE
- IMG
- JPEG
- JPEG 2000
- NITF
- PNG
- RPF
- SRTM1, 2

The raster used in this sample is a DTED2 file.

## Relevant API

* RasterElevationSource
* Surface

## Offline data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[MontereyElevation Raster](https://www.arcgis.com/home/item.html?id=98092369c4ae4d549bbbd45dba993ebc)| `<userhome>`/ArcGIS/Runtime/Data/raster/MontereyElevation.dt2 |

## Tags

3D, Raster, Elevation, Surface
