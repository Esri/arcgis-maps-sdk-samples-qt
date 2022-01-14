# Display dimensions

Display dimension features from a mobile map package.

![](screenshot.png)

## Use case

Dimensions show specific lengths or distances on a map. A dimension may indicate the length of a side of a building or land parcel, or the distance between two features, such as a fire hydrant and the corner of a building.

## How to use the sample

When the sample loads, it will automatically display the map containing dimension features from the mobile map package. The name of the dimension layer containing the dimension features is displayed in the controls box. Control the visibility of the dimension layer with the "Dimension Layer visibility" check box, and apply a definition expression to show dimensions greater than or equal to 450m in length using the "Definition Expression" check box.

**Note**: the minimum scale range of the sample is set to 1:35000 to maintain readability of the dimension features.

## How it works

1. Create a `MobileMapPackage` specifying the path to the .mmpk file.
2. Load the mobile map package.
3. After the mmpk successfully loads, get the map from the mmpk `m_map = m_mmpk->maps().at(0)` and add it to the map view: `m_mapView->setMap(m_map)`.
4. Loop through the map's layers to find the `DimensionLayer` and assign it to `m_dimensionLayer`.
5. Set the title of the controls box using `m_dimensionLayer->name()`.
6. Control the dimension layer's visibility with `m_dimensionLayer->setVisible(boolean)` and set a definition expression with `m_dimensionLayer->setDefinitionExpression(String)`.

## Relevant API

* DimensionLayer
* MobileMapPackage

## About the data

This sample shows a subset of the network of pylons, substations, and power lines around Edinburgh, Scotland within an [Edinburgh Pylon Dimensions mobile map package](https://arcgis.com/home/item.html?id=f5ff6f5556a945bca87ca513b8729a1e). The data was digitised from satellite imagery and is intended to be used for illustrative purposes only.

## Additional information

Dimension layers can be taken offline from a feature service hosted on ArcGIS Enterprise 10.9 or later, using the [GeodatabaseSyncTask](https://developers.arcgis.com/java/api-reference/reference/com/esri/arcgisruntime/tasks/geodatabase/GeodatabaseSyncTask.html). Dimension layers are also supported in mobile map packages or mobile geodatabases created in ArcGIS Pro 2.9 or later.

## Tags

definition expression, dimension, distance, layer, length, mmpk, mobile map package, utility
