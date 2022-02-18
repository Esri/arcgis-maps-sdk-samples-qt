# Show labels on layer in 3D

This sample demonstrates how to display custom labels in a 3D scene.

![](screenshot.png)

## Use case

Labeling features is useful to visually display information or attributes on a scene. For example, city officials or maintenance crews may want to show installation dates of features of a gas network.

## How to use the sample

Pan and zoom to explore the scene. Notice the labels showing installation dates of features in the 3D gas network.

## How it works

1. Create an `Scene` from a `PortalItem`.
2. Add the scene to an `SceneView` and load it.
3. After loading is complete, obtain the `FeatureLayer` from one of the `GroupLayer`s in the scene's `operationalLayers`.
4. Create a `TextSymbol` to define how labels are stylized.
5. After the `Scene` has loaded, obtain the `FeatureLayer` from the scene's `operationalLayers`.
6. Create an `LabelDefinition` using an  `ArcadeLabelExpression`.
* Set the "labelExpressionInfo.expression" key to define what text the label should display. You can use fields of the feature by using `$feature.NAME` in the expression.
7. Add the definition to the feature layer's `labelDefinitions` array.

## Relevant API

* ArcadeLabelExpression
* FeatureLayer
* LabelDefinition
* Scene
* SceneView
* TextSymbol

## About the data

This sample shows a [New York City infrastructure](https://www.arcgis.com/home/item.html?id=850dfee7d30f4d9da0ebca34a533c169) scene hosted on ArcGIS Online.

## Tags

3D, arcade, attribute, buildings, label, model, scene, symbol, text, URL, visualization

