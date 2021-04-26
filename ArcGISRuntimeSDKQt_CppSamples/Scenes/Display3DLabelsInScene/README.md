# Display 3D labels in scene

This sample demonstrates how to display custom labels in a 3D scene.

![](screenshot.png)

## Use case

Labeling features is useful to visually display information or attributes on a scene. For example, city officials or maintenance crews may want to show installation dates of features of a gas network.

## How to use the sample

Pan and zoom to explore the scene. Notice the labels showing installation dates of features in the 3D gas network.

## How it works

1. Create a `Scene` from a URL.
2. Obtain the `FeatureLayer` from one of the `GroupLayer`s in the scene's `operationalLayers`.
3. Create a `TextSymbol` to use for displaying the label text.
4. Create a `LabelDefinition` with an `ArcadeLabelExpression`.
5. Add the definition to the feature layer's `labelDefinitions` array.
6. Set the feature layer's `labelsEnabled` property to `true`.

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

3D, attribute, buildings, label, model, scene, symbol, text, URL, visualization

