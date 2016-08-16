#Offline geocode

This sample demonstrates how to Geocode and Reverse Geocode addresses and locations without network connectivity.

![](screenshot.png)

##How it works
An `ArcGISTiledLayer` and `LocatorTask` are created using local files. Geocoding is performed through inputs from a `TextField`. ReverseGeocoding is handled with various mouse events. Information from geocoding tasks is visually displayed through a `PictureMarkerSymbol` graphic and a `Callout`. 

##Features
- Map
- MapView
- ArcGISTiledLayer
- TileCache
- GraphicsOverlay
- PictureMarkerSymbol
- Callout
- LocatorTask
- GeocodeParameters
- ReverseGeocodeParameters
- GeocodeResult
- SuggestListModel
- Viewpoint
