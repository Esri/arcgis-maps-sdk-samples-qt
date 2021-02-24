# Display 3D labels in scene

This sample demonstrates how to display custom labels in a 3D scene.
Labeling features is useful to visually display information or attributes on a scene. For example, city officials or maintenance crews may want to show installation dates of features of a gas network.

![](screenshot.png)

## How to use the sample
Pan and zoom to explore the scene. Notice the labels showing installation dates of features in the 3D gas network.

## How it works
1. Create a `Scene` using the `initUrl` property.
2. Create a `TextSymbol` to use for displaying the label text.
3. Create a `LabelDefinition` with a `json` property
    * Set the "labelExpressionInfo.expression" key to create an Arcade expression that defines what text the label should display. You can use fields of the feature by using $feature.FIELD_NAME in the expression.
    * To use a text symbol, set the "symbol" key in the JSON representation using `TextSymbol.json`.
4. Create a label definition from the JSON using `LabelDefinition::fromJson()`.
5. After the `Scene` has loaded, obtain the `FeatureLayer` from the scene's `operationalLayers`.
6. Set the `labelsEnabled` property to `true`.
7. Add the label definition to the feature layer's `labelDefinitions` list model.

## Relevant API

* FeatureLayer
* LabelDefinition
* Scene
* SceneView
* TextSymbol

## About the data

This sample shows a [New York City infrastructure](https://www.arcgis.com/home/item.html?id=850dfee7d30f4d9da0ebca34a533c169) scene hosted on ArcGIS Online.

## Tags

3D, attribute, buildings, label, model, scene, symbol, text, URL, visualization

