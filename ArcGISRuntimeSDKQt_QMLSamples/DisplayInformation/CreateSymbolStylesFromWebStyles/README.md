# Create symbol styles from web styles

Create symbol styles from a style file hosted on a portal.

![](screenshot.png)

## Use case

Style files hosted on an ArcGIS Online or Enterprise portal are known as web styles. They can be used to style symbols on a feature layer or graphic overlay. Since styles are published from ArcGIS Pro, you can author and design your own beautiful multilayer vector symbols. These vector symbols look good at any resolution and scale well. Users can now access these styles from their native applications, and make use of the vector symbols within them to enhance features and graphics in the map.

## How to use the sample
The sample displays a map with a set of symbols that represent the categories of the features within the dataset. Pan and zoom on the map and view the legend to explore the appearance and names of the different symbols from the selected symbol style.

## How it works
1. Create a `FeatureLayer` and add it to the map.
2. Create a `UniqueValueRenderer` and add it to the feature layer.
3. Create a `SymbolStyle` from a portal by passing in the web style name and portal URL.
       * Note: passing `null` as the portal will default to ArcGIS.com.
4. Create `SearchSymbolParameters` and set the `keys` parameter to the names of the symbols you want to display.
5. Search for symbols in the `SymbolStyle` by name using `symbolStyle->searchSymbols(symbolStyleSearchParameters)`.
6. Call `fetchSymbol` on each `SymbolStyleSearchResult` in the returned `SymbolStyleSearchResultListModel` to generate a symbol for each search key.
7. Create `UniqueValue` objects for each symbol with defined values to map the symbol to features on the feature layer.
8. Add each `UniqueValue` to the `UniqueValueRenderer`.

## Relevant API
* FeatureLayer
* Symbol
* SymbolStyle
* UniqueValue
* UniqueValueRenderer

## About the data

The sample uses the ['Esri2DPointSymbolsStyle'](https://developers.arcgis.com/javascript/latest/guide/esri-web-style-symbols-2d) Web Style.

The map shows features from the [LA County Points of Interest](https://www.arcgis.com/home/item.html?id=b9f7c339f9d747558329f44059064ccc) service hosted on ArcGIS Online.

## Additional information

2D web styles, dictionary web styles, and 3D web styles can all be hosted on an ArcGIS Online or Enterprise portal.

## Tags

renderer, symbol, symbology, web style
