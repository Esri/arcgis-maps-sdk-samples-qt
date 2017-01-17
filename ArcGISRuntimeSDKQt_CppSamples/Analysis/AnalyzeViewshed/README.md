#Analyze viewshed

This sample demonstrates how to use GeoprocessingTask to calculate a viewshed using a geoprocessing service. Click any point on the map to see all areas that are visible within a 1 kilometer radius. It may take a few seconds for the model to run and send back the results.

![](screenshot.png)

##How it works
A `GeoprocessingTask` with the URL set to the REST endpoint of a geoprocessing service is created. A signal handler is set up for the `mouseClicked` signal on the `MapView`, and when this signal emits, a `Graphic` is added to the `Map`. Once the graphic is added to the `Map`, a `FeatureCollectionTable` is created, a new `Feature` is added from the graphic's location into the `FeatureCollectionTable`, and new `GeoprocessingParameters` are created using the `FeatureCollectionTable`. Next, a `GeoprocessingJob` is created from the `GeoprocessingTask` by passing in the parameters. Once the job is obtained, a signal handler is created to handle the asynchronous status updates and the job is started. Once the job completes successfully, a `FeatureSet` is obtained from the `GeoprocessingResult`, and the features from the `FeatureSet` are added to a `GraphicsOverlay`.

##Features
- GeoprocessingTask
- GeoprocessingParameters
- GeoprocessingJob
- GeoprocessingFeatures
- GeoprocessingResult
- FeatureCollectionTable
- GraphicsOverlay
- SimpleRenderer
- Graphic
- SimpleMarkerSymbol
- SimpleFillSymbol
- FeatureIterator
- MapView
- Map
- Basemap
