# Build a legend

Build a legend for all the operational layers in the map.

![](screenshot.png)

## Use case

Legends are used to describe what each symbol on a map represents. A common format is to show an image of the symbol alongside of a text description of what that symbol represents. This sample demonstrates how a simple legend can be built up in code using the `LayerContent` interface.

## How to use the sample

1. Open the sample
2. Flick through the legend control to see the various elements that represent features on the map.

## How it works

1. Layers implement the `LayerContent` interface, which contain a list of `LegendInfo`. `LegendInfo` contains a `Symbol` and a name string. `Layer::setAutoFetchLegendInfos` must be called on each `Layer` instance to fetch the info from the data.
2. Connect to the `fetchLegendInfosCompleted` signal and once completed use the `LegendInfoListModel` to build the legend.
3. The names and images are then displayed next to each other in a list view.

## Relevant API

* Layer::setAutoFetchLegendInfos
* LayerContent
* LegendInfo
* LegendInfoListModel

## Tags

legend, legend info, symbol swatch, toolkit
