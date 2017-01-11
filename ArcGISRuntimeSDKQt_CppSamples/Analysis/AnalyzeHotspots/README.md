#Analyze hotspots

This sample demonstrates how to execute a geoprocessing task to calculate a hotspot analysis based on the frequency of 911 calls. It calculates the frequency of these calls within a given study area during a specified constrained time period set between 1/1/1998 and 5/31/1998.

![](screenshot.png)

##How it works
A `GeoprocessingTask` is created by setting the URL to the REST endpoint of a geoprocessing service. `GeoprocessingParameters` are created and the execution type is set to `AsynchronousSubmit`. Once the date ranges are selected, a query string is created with the to and from dates. A key/value parameter input is created, where the key is the string "Query" and the value is a `GeoprocessingString` that contains the query string. These parameter inputs are then added to the `GeoprocessingParameters`. A `GeoprocessingJob` is obtained by calling the `createJob` method of the `GeoprocessingTask`. The job is started, and once it is complete, the `ArcGISMapImageLayer` is obtained from the result, and added to the `Map`.

##Features
- GeoprocessingTask
- GeoprocessingJob
- GeoprocessingParameters
- GeoprocessingString
- GeoprocessingResult
- ArcGISMapImageLayer
- Map
