# Style WMS layer

This sample demonstrates how to dynamically change the style of a sublayer in a WMS layer.

![](screenshot.png)

## How to use the sample
Select one of the styles and the map will update the display with the selected style.

## How it works
- A `WmsSublayer` is obtained from a `WmsLayer` once the `WmsLayer` is loaded.
- A string list of available styles is obtained from the `WmsSublayer's` sublayerInfo (`WmsLayerInfo`).
- The sublayer's currentStyle is set to one of the style strings that was obtained from the `WmsLayerInfo`.

## Relevant API
- WmsLayer
- WmsSublayer
- WmsLayerInfo
- WmsSublayer.currentStyle

## Tags
WMS Style, Change Style
