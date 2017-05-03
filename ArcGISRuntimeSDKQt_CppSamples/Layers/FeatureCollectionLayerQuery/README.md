# Feature collection layer (query)

This samples demonstrates how to create a feature collection layer to show a query result from a service feature table.

![](screenshot.png)

## How it works

- A query is performed using the `queryFeatures()` method on a `ServiceFeatureTable`.
- The result of the query is used to instantiate an `FeatureCollectionTable`.
- The table is added to the list of tables belonging to a `FeatureCollection`
- The feature collection is then passed to a `FeatureCollectionLayer`.
- The layer is then displayed on the map by adding it to the `operationalLayers` list.

## Features

- `FeatureCollection`
- `FeatureCollectionLayer`
- `FeatureCollectionTable`
- `Map`
- `MapQuickView`
- `ServiceFeatureTable`

