# Service feature table (manual cache)

Display a feature layer from a service using the **manual cache** feature request mode.

![](screenshot.png)

## Use case

`ServiceFeatureTable` supports three request modes, which define how features are requested from the service and stored in the local table. The feature request modes have different performance characteristics. Use **manual cache** in scenarios where you want to explicitly control requests for features.

## How to use the sample

Run the sample and pan and zoom around the map. No features will be requested and displayed automatically. Press Populate, and features will display.

## How it works

1. Set the `ServiceFeatureTable::setFeatureRequestMode` property of the service feature table to `FeatureRequestMode::ManualCache` before the table is loaded.
2. Load the table.
3. Call `populateFromServiceAsync` on the table to request features.

## Relevant API

* FeatureLayer
* FeatureRequestMode::ManualCache
* ServiceFeatureTable
* ServiceFeatureTable::populateFromServiceAsync
* ServiceFeatureTable::setFeatureRequestMode

## About the data

The samples uses an [incident feature layer](https://sampleserver6.arcgisonline.com/arcgis/rest/services/SF311/FeatureServer/0) queried to show tree maintenance or damage. The sample opens with an initial visible extent centered over San Francisco.

## Additional information

In **manual cache** mode, features are never automatically populated from the service. All features are loaded manually using calls to `PopulateFromService`.

## Tags

cache, feature request mode, performance
