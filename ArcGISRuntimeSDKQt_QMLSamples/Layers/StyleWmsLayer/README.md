# Style WMS layer

Change the style of a Web Map Service (WMS) layer.

![](screenshot.png)

## Use case

Layers hosted on WMS may have different pre-set styles available to apply to them. Swapping between these styles can help during visual examination of the data. For example, increasing the contrast of satellite images can help in identifying urban and agricultural areas within forested areas.

## How to use the sample

Select one of the styles and the map will update the display with the selected style.

## How it works

1. Create a `WmsLayer` specifying the URL of the service and the layer names you want.
2. When the layer is done loading, get its list of style strings using `wmsLayer.sublayers[0].sublayerInfo.styles`.
3. Set one of the styles using `wmsLayer.sublayers[0].currentStyle = styleString`.

## Relevant API

* WmsLayer
* WmsSublayer
* WmsSublayerInfo

## About the data

This sample uses a public service managed by the State of Minnesota and provides composite imagery for the state and the surrounding areas.

## Tags

imagery, styles, visualization, WMS