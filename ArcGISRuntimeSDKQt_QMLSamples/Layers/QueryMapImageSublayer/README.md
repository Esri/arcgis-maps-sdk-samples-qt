# Query map image sublayer

Find features in a sublayer based on attributes and location.

![](screenshot.png)

## Use case

Sublayers of an `ArcGISMapImageLayer` may expose a `ServiceFeatureTable` through a `table` property. This allows you to perform the same queries available when working with a table from a `FeatureLayer`: attribute query, spatial query, statistics query, query for related features, etc. An image layer with a sublayer of counties can be queried by population to only show those above a minimum population.

## How to use the sample

Specify a minimum population in the input field (values under 1810000 will produce a selection in all layers) and click the query button to query the sublayers in the current view extent. After a short time, the results for each sublayer will appear as graphics.

## How it works

1. Create an `ArcGISMapImageLayer` object using the URL of an image service.
2. After loading the layer, get the sublayer you want to query with `(ArcGISMapImageSublayer) layer.mapImageSublayers.get(index)`.
3. Load the sublayer, and then get its `ServiceFeatureTable` with `sublayer.table`.
4. Create `QueryParameters`. You can use `queryParameters.whereClause = sqlQueryString` to query against a table attribute and/or set `queryParameters.geometry = extent` to limit the results to an area of the map.
5. Call `sublayerTable.queryFeatures(queryParameters)` to get a `FeatureQueryResult` with features matching the query. The result is an iterable of features.

## Relevant API

* ArcGISMapImageLayer
* ArcGISMapImageLayer.loadTablesAndLayers
* ArcGISMapImageSublayer
* ArcGISMapImageSublayer.table
* QueryParameters
* ServiceFeatureTable

## Additional information

An `ArcGISMapImageSublayer` must be loaded before accessing its metadata or table. Use `ArcGISMapImageLayer.loadTablesAndLayers` to recursively load all sublayers and tables associated with a map image layer. Some sublayers do not have an associated table (group layers, for example) and some may not support specific types of queries. Consult the map service metadata for details.

## Tags

MapServer, Query, Sublayer, Table

