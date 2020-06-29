# Show a popup

Show predefined popups from a web map.

![](screenshot.png)

## Use case

Many web maps contain predefined popups which are used to display the attributes associated with each feature layer in the map, such as hiking trails, land values, or unemployment rates. You can display text, attachments, images, charts, and web links. Rather than creating new popups to display information, you can easily access and display the predefined popups.

## How to use the sample

Tap on the features to prompt a popup that displays information about the feature.

## How it works

1. Create and load an `Map` using a URL.
2. Set the map to an `MapView` and set the `touchDelegate`.
3. Use the `GeoView.identifyLayer(_:screenPoint:tolerance:returnPopupsOnly:completion:)` method to identify the top-most feature.
4. Create an `PopupsViewController` with the result's `popups`.
5. Present the view controller.

## Relevant API

* IdentifyLayerResult
* Map
* PopupsViewController

## About the data

This sample uses a [feature layer](https://sampleserver6.arcgisonline.com/arcgis/rest/services/SF311/FeatureServer/0) that displays reported incidents in San Francisco.

## Tags

feature, feature layer, popup, web map
