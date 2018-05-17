# Show labels on layers

This sample demonstrates how to display labels on a FeatureLayer. For example, you may want to label street names on a streets layer. Using the ArcGIS [JSON specification for labels](https://developers.arcgis.com/web-map-specification/objects/labelingInfo/), you can control the content and display properties of a label.

![](screenshot.png)

## How to use the sample
Open the sample. Pan and zoom around the United States. Red polylines representing highways are displayed on the map. As you navigate the map, labels indicating the interestate name will display as blue text with a yellow halo. The label will contain the characters "I - ", followed by the interstate route number.

## How it works
1. A `FeatureLayer` and `ServiceFeatureTable` are created by passing in a URL to the REST endpoint of a map service.
2. A label JSON is created using the ArcGIS [JSON specification for labels](https://developers.arcgis.com/web-map-specification/objects/labelingInfo/).
3. A `LabelDefinition` is created from the label JSON.
4. The `LabelDefinition` is added to the list of `LabelDefinitions` in the `FeatureLayer` instance.
5. Labels are enabled on the `FeatureLayer` by setting `labelsEnabled` to true.

## Relevant API
 - LabelDefinition
 - FeatureLayer

## Tags
Labels
