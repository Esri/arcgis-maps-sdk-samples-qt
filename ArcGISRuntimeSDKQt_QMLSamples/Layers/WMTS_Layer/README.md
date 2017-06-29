# WMTS layer

This sample demonstrates how to load a Web Map Tile Service (WMTS) and display it as a layer in a Map.

![](screenshot.png)

## How it works
WMTS is a type of web map service developed by the Open Geospatial Consortium (OGC). To consume in ArcGIS Runtime, first create a `WmtsService` by passing in the URL to the WMTS service. Load the `WmtsService` by calling `load`, and wait for the service to load by connecting to the `loadStatusChanged` signal. Once the service loads, obtain the `WmtsServiceInfo` and the list of `WmtsLayerInfo` from the loaded `WmtsService`. In this particular sample, the first layer is obtained from the list of `WmtsLayerInfo`, and that layer's ID is retrieved. Finally, a `WmtsLayer` is created by setting the URL and the layer ID that was obtained in the previous step. A `Basemap` is created from the `WmtsLayer`, and this Basemap is added to a new Map.

## Features
- WmtsLayer
- WmtsLayerInfo
- WmtsService
- WmtsServiceInfo
- Basemap
- Map
- MapView
