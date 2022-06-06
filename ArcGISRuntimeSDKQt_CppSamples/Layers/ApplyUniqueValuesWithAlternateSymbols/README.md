# Apply unique values with alternate symbols

This sample demonstrates how to create a unique value with alternate symbols.

![](screenshot.png)

## Use case

When a layer is symbolized with unique value symbology, you can specify the visible scale range for each unique value. This is an effective strategy to limit the amount of detailed data at smaller scales without having to make multiple versions of the layer, each with a unique definition query.

Once scale ranges are applied to unique values, you can further refine the appearance of features within those scale ranges by establishing alternate symbols to different parts of the symbol class scale range.

## How to use the sample

From the UI, select one of the predetermined scales (1:2500, 1:7500 or 1:15000) to view the point symbology at that scale in different colors and geometries which is dependent on the scale. In addition to using the predetermined buttons, you can also manually zoom in an out of the map to see alternate symbols in action dynamically.

## How it works

This sample uses a unique value renderer that is created with a unique value that has a list of alternate symbols and their corresponding reference properties to specify at which scales each symbol should appear. Once a unique value renderer with alternate sybmols is created, it is assigned as the renderer of the feature layer.

1. Create a feature layer using the service url and add it to the list of operational layers.
2. Create two alternate symbols (a blue square and a yellow diamond) to be used for the unique value. To create an alternate symbol:

    a. Create a symbol using `SimpleMarkerSymbol`.
    
    b. Convert the Simple Marker Symbol to a Multilayer Symbol.
    
    c. Set the valid scale range through reference properties on the multilayer point symbols blue square and yellow diamond by calling `MultilayerSymbol::setReferenceProperties(new SymbolReferenceProperties(double minScale, double maxScale, QObject *parent = nullptr));`.
    
3. Create a third multilayer symbol to be used when creating the unique value:
    
    a. Create a red triangle and set the reference properties.
    
4. Create a unique value using the red triangle from step three and the list of alternate symbols from step two.
5. Create a unique value renderer and add the unique value from step four to it.
6. Create a purple diamond simple marker and convert it to a multilayer symbol to be used as the default symbol.
7. Set the default symbol on the unique value renderer to the purple diamond from step six.
8. Set the fieldName on the unique value renderer to "req_type".
9. Assign this unique value renderer as the renderer on the feature layer from step one.

## Relevant API

* MultilayerSymbol
* SimpleMarkerSymbol
* SymbolReferenceProperties
* UniqueValue
* UniqueValueRenderer

## Tags

alternate symbols, scale based rendering, unique value, unique value renderer

