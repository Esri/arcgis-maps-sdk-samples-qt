# Export vector tiles

Export tiles from an online vector tile service.

![](screenshot.png)

## Use case

Field workers with limited network connectivity can use exported vector tiles as a basemap for use while offline.

## How to use the sample

When the vector tiled layer loads, zoom in to the extent you want to export. The red box shows the extent that will be exported. Click the "Export Vector Tiles" button to start the job. When finished, a dialog will show the exported result as a new basemap.

## How it works

1. Create an `ArcGISVectorTiledLayer` from the map's base layers.
2. Create an `ExportVectorTilesTask` using the vector tiled layer's URL.
3. Create default `ExportVectorTilesParameters` from the task, specifying extent and maximum scale.
4. Create a `ExportVectorTilesJob` from the task using the parameters, and specifying a vector tile cache path and an item resource path. The resource path is required if you want to export the tiles with the style.
5. Start the job, and once it completes successfully, get the resulting `ExportVectorTilesResult`.
6. Get the `VectorTileCache` and `ItemResourceCache` from the result to create an `ArcGISVectorTiledLayer` that can be displayed to the map view.

## Relevant API

* ArcGISVectorTiledLayer
* ExportVectorTilesJob
* ExportVectorTilesParameters
* ExportVectorTilesResult
* ExportVectorTilesTask
* ItemResourceCache
* VectorTileCache

## Additional information

**NOTE:** Downloading tiles for offline use requires authentication with the web map's server. To use this sample, you will require an [ArcGIS Online account](https://www.arcgis.com/).

Vector tiles have high drawing performance and smaller file size compared to regular tiled layers due to consisting solely of points, lines, and polygons. However, in ArcGIS Runtime SDK they cannot be displayed in scenes. Visit the [ArcGIS Online Developer's portal](https://developers.arcgis.com/qt/layers/layer-types/) to learn more about the characteristics of different layer types.

## Tags

cache, download, offline, vector, vector tiled layer
