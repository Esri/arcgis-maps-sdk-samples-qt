# Custom dictionary style

Use a custom dictionary created from a web style or style file (.stylx) to symbolize features using a variety of attribute values.

![](screenshot.png)

## Use case

When symbolizing geoelements in your map, you may need to convey several pieces of information with a single symbol. You could try to symbolize such data using a unique value renderer, but as the number of fields and values increases, that approach becomes impractical. With a dictionary renderer you can build each symbol on-the-fly, driven by one or more attribute values, and handle a nearly infinite number of unique combinations.

## How to use the sample

Toggle between the dictionary symbols from the web style and style file. Pan and zoom around the map to see the symbology from the chosen dictionary symbol style.

## How it works

1. Create a new `DictionarySymbolStyle` for both the local and web dictionary symbol styles.
    * Web style: use `DictionarySymbolStyle::DictionarySymbolStyle()` or `DictionarySymbolStyle::DictionarySymbolStyleFromUrl()` by passing a `PortalItem` or the url to the hosted style.
    * Local style file: use `DictionarySymbolStyle::createFromFile()` and pass the path to the file on the device.
2. Create a new `DictionaryRenderer` for each, providing the dictionary symbol style.
3. Apply the dictionary renderer to a feature layer using `layer::setRenderer(dictionaryRenderer)`.

## Relevant API

* DictionaryRenderer
* DictionarySymbolStyle

## About the data

The data used in this sample is from a feature layer showing a subset of [restaurants in Redlands, CA](https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/rest/services/Redlands_Restaurants/FeatureServer) hosted as a feature service with attributes for rating, style, health score, and open hours.

## Additional information

For information about creating your own custom dictionary style, see the open source [dictionary-renderer-toolkit](https://esriurl.com/DictionaryToolkit).

## Tags

dictionary, military, renderer, style, stylx, unique value, visualization
