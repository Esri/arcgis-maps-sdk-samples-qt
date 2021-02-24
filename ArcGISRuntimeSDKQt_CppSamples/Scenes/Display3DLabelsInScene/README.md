# Display 3D labels in scene

This sample demonstrates how to display custom labels in a 3D scene.
Labeling features is useful to visually display information or attributes on a scene. For example, city officials or maintenance crews may want to show installation dates of features of a gas network.

![](screenshot.png)

## How to use the sample
1. Create an `Scene` using a URL.
2. ~~Apply the scene to a `SceneView` and load it.~~
3. After the scene has loaded, obtain the `FeatureLayer` from the scene's `operationalLayers`.
4. Use `setLabelsEnabled()` to set the `labelsEnabled` property to `true`.
5. Create a TextSymbol to use for displaying the label text.
6. Create a JSON string for the label definition.
7. Set the "labelExpressionInfo.expression" key to define what text the label should display. You can use fields of the feature by using $feature.NAME in the expression.
8. To use a text symbol, set the "symbol" key in the JSON representation using AGSTextSymbol.toJSON().
9. Create a label definition from the JSON using AGSJSONSerializable.fromJSON(_:).
10. Add the definition to the feature layer's labelDefinitions array.

## How it works
e.g. In the `GeoView.Tapped` event, features in the `Map` are selected using an `Envelope` defined by the user's tap location ...

## Relevant API
 - ClassName1
 - MethodName

## Offline data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[San Francisco Streets TPK](https://www.arcgis.com/home/item.html?id=3f1bbf0ec70b409a975f5c91f363fe7d)| `<userhome>`/ArcGIS/Runtime/Data/tpk/SanFrancisco.tpk |

## Additional information
A standard level license is required to ...

## Tags
Routing, Network analysis, Geocode

