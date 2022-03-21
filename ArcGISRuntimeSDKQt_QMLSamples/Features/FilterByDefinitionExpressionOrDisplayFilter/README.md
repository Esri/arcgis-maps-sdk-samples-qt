# Filter by definition expression or display filter

Filter features displayed on a map using a definition expression or a display filter.

![](screenshot.png)

## Use case

Definition queries allow you to define a subset of features to work with in a layer by filtering which features are retrieved from the dataset by the layer. This means that a definition query affects not only drawing, but also which features appear in the layer's attribute table and therefore which features can be selected, labeled, identified, and processed by geoprocessing tools.

Alternatively, display filters limit which features are drawn, but retain all features in queries and when processing. Definition queries and display filters can be used together on a layer, but definition queries limit the features available in the layer, while display filters only limit which features are displayed.

In this sample you can filter a dataset of tree quality selecting for only those trees which require maintenance or are damaged.

## How to use the sample

Press 'Apply Expression' to limit the features requested from the feature layer to those specified by the SQL query definition expression.
Press 'Apply Display Filter' to limit the features shown on the feature layer to those specified by the SQL query without modifying the feature table.
Click 'Reset' to remove the definition expression on the feature layer, which returns all the records.

## How it works

1. Create a service feature table from a URL.
2. Create a feature layer from the service feature table.
3. Filter features on your feature layer using a `DefinitionExpression` to view a subset of features and modify the attribute table.
4. Filter features on your feature layer using a `DisplayFilter` to view a subset of features without modifying the attribute table.

## Relevant API

* DefinitionExpression
* DisplayFilter
* FeatureLayer
* ServiceFeatureTable

## About the data

This map displays point features related to crime incidents such as grafitti and tree damage that have been reported by city residents.

## Tags

definition expression, display filter, filter, limit data, query, restrict data, SQL, where clause
