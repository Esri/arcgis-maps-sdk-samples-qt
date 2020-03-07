# Read symbols from a mobile style

Combine multiple symbols from a mobile style file into a single symbol.

![](screenshot.png)

## Use case

You may choose to display individual elements of a dataset like a water infrastructure network (such as valves, nodes, or endpoints) with the same basic shape, but wish to modify characteristics of elements according to some technical specifications. Multilayer symbols lets you add or remove components or modify the colors to create advanced symbol styles.

## How to use the sample

Select a symbol and a color from each of the category lists to create an emoji. A preview of the symbol is updated as selections are made. The size of the symbol can be set using the slider. Click the map to create a point graphic using the customized emoji symbol, and click `Clear` to clear all graphics from the display.

## How it works

1. Create a new `SymbolStyle` from a stylx file, and load it.
2. Get a list of symbols in the style by calling `SymbolStyle::searchSymbols`.
3. Display the resulting `SymbolStyleSearchResultListModel` inside a series of ComboBoxes.
4. When symbol selections change, create a new multilayer symbol by passing the keys for the selected symbols into `SymbolStyle::fetchSymbol`.
5. Iterate through the symbol layers and color lock all symbol layers except the base layer and update the current symbol preview image by calling `Symbol::createSwatch`.
6. Create graphics symbolized with the current symbol when the user taps the map view.

## Relevant API

* MultilayerPointSymbol
* Symbol::createSwatch
* SymbolLayer
* SymbolStyle
* SymbolStyle::searchSymbols
* SymbolStyle::fetchSymbol
* SymbolStyleSearchResultListModel
* SymbolStyleSearchResult
* SymbolStyleSearchParameters

## Offline Data

A mobile style file (created using ArcGIS Pro) provides the symbols used by the sample.

Link | Local Location
---------|-------|
|[Emoji mobile style](https://www.arcgis.com/home/item.html?id=1bd036f221f54a99abc9e46ff3511cbf)| `<userhome>`/ArcGIS/Runtime/Data/style/emoji-mobile.stylx |

## About the data

The mobile style file used in this sample was created using ArcGIS Pro, and is hosted on [ArcGIS Online](https://www.arcgis.com/home/item.html?id=1bd036f221f54a99abc9e46ff3511cbf). It contains symbol layers that can be combined to create emojis.

## Additional information

While each of these symbols can be created from scratch, a more convenient workflow is to author them using ArcGIS Pro and store them in a mobile style file (.stylx). ArcGIS Runtime can read symbols from a mobile style, and you can modify and combine them as needed in your app.

## Tags

advanced symbology, mobile style, multilayer, stylx
