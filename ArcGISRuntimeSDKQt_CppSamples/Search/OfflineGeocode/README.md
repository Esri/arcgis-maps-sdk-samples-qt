#Offline geocode

This sample demonstrates how to Geocode and Reverse Geocode addresses and locations without network connectivity.

![](screenshot.png)

##How it works
An `ArcGISTiledLayer` and `LocatorTask` are created using local files. Geocoding is performed with inputs from a QtQuick `TextField`. Reverse geocoding is handled through various mouse events. Information from geocoding tasks is visually displayed through a `PictureMarkerSymbol` graphic and a `Callout`. 

##Features
- Map
- MapView
- ArcGISTiledLayer
- TileCache
- Callout
- LocatorTask
- GeocodeResult
- GeocodeParameters
- ReverseGeocodeParameters
- SuggestListModel
- GraphicOverlay
- PictureMarkerSymbol
- Viewpoint
