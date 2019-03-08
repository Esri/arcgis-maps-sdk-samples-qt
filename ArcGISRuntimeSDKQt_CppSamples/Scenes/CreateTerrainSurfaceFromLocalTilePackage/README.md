# Create terrain surface from a local tile package

Set the terrain surface with elevation described by a local tile package.

The terrain surface is what the basemap, operational layers, and graphics are draped on.

![](screenshot.png)

## How it works

1. Create a `Scene` and add it to a `SceneView`.
2. Create an `ArcGISTiledElevationSource`  from a local tile package (.tpk)
3. Add this source to the scene's base surface: `Scene.BaseSurface.ElevationSources.append(tileElevationSource)`

## Additional information

See the [ArcGIS Pro Tile Package documentation](https://pro.arcgis.com/en/pro-app/help/sharing/overview/tile-package.htm) (since Esri.ArcGISRuntime 100.1) for information on tile packages
## Relevant API

* ArcGISTiledElevationSource
* Surface

## Offline data
The tile package must be a LERC (limited error raster compression) encoded TPK. Details on creating these are in the [ArcGIS Pro documentation](https://pro.arcgis.com/en/pro-app/help/sharing/overview/tile-package.htm)

Link | Local Location
---------|-------|
|[MontereyElevation Tile Package](https://www.arcgis.com/home/item.html?id=cce37043eb0440c7a5c109cf8aad5500)| `<userhome>`/ArcGIS/Runtime/Data/tpk/MontereyElevation.tpk |

## Tags

3D, Tile Cache, Elevation, Surface
