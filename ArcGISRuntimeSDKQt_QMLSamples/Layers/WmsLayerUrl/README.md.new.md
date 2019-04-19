# WMS layer (URL)

This sample demonstrates how to display a WMS Layer from an online URL.

![](screenshot.png)

## How it works

A `WmsLayer` is declared inside a `Map` by setting its URL and
layerNames properties. The URL must be the URL to the “GetCapabilities”
endpoint of the WMS Service. Alternatively, the layer could be
constructed by setting the layerInfos property.
