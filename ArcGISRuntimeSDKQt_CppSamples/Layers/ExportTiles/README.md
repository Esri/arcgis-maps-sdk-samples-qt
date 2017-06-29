# Export tiles

This sample demonstrates how to export tiles from a map service for offline use.

![](screenshot.png)

## How it works

Pan and zoom into the desired area, making sure the area you want to take offline is within the red rectangle. Tap on the `Export tiles` button, and this will obtain the default parameters and call `exportTileCacheWithParameters`, which will return a job. The job must be started manually, and once started can be canceled at anytime. Once the job completes successfully, the output tile cache is added as an `ArcGISTiledLayer` to the map.


## Features
- Map
- MapView
- ArcGISTiledLayer
- TileCache
- ExportTileCacheTask
- ExportTileCacheJob
- ExportTiledCacheParameters
