# Display 3D labels in scene

This sample demonstrates how to display custom labels in a 3D scene.

![](screenshot.png)

## Use case

Labeling features is useful to visually display information or attributes on a scene. For example, city officials or maintenance crews may want to show installation dates of features of a gas network.

## How to use the sample
Pan and zoom to explore the scene. Notice the labels showing installation dates of features in the 3D gas network.

## How it works
1. Create a `Scene` from a URL.
2. Create a `LabelDefinition` to define how the labels will be displayed.
3. Create an `ArcadeLabelExpression` to define the labels' format.
4. Create a `TextSymbol` to define how labels are stylized.
5. After the `Scene` has loaded, obtain the `FeatureLayer` from the scene's `operationalLayers`.
6. Set the `labelsEnabled` property to `true`.
7. Add the label definition to the feature layer's `labelDefinitions` list model.

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

