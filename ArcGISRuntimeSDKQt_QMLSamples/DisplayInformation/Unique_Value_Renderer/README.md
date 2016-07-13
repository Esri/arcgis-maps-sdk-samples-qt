#UniqueValueRenderer

This sample demonstrates how to use a UniqueValueRenderer to style different Features in a FeatureLayer with different Symbols.

![](screenshot.png)

##How it works
First a `ServiceFeatureTable` and a `FeatureLayer` are constructed and added to the Map. Then a `UniqueValueRenderer` is created, and the fieldName to be used as the renderer field is set as ("STATE_NAME"). You can use multiple fields; this sample only uses one. Multiple `SimpleFillSymbol`s are defined for each type of feature we want to render differently (in this case different states of the USA). `SimpleFillSymbol`s can be applied to polygon features; these are the types of features found in the feature service used for this `ServiceFeatureTable`. A default symbol is also created; this will be used for all other features that do not match the all of the `UniqueValue`s defined. Separate `UniqueValue`s objects are created which define the values in the renderer field and what symbol should be used for matching features. These are added to the `UniqueValue` list. The renderer is set on the layer and is rendered in the `MapView` accordingly.

##Features
- Map
- MapView
- Basemap
- FeatureLayer
- ServiceFeatureTable
- UniqueValueRenderer
- UniqueValue
- SimpleFillSymbol
- SimpleLineSymbol
- Viewpoint
- Envelope
- SpatialReference
