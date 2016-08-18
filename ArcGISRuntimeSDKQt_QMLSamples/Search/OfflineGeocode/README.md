#Offline geocode

This sample demonstrates how to geocode and reverse geocode addresses and locations without network connectivity.

![](screenshot.png)

##How it works
An `ArcGISTiledLayer` and `LocatorTask` are created using local files. Geocoding is performed through inputs from a `TextField`. ReverseGeocoding is handled with various mouse events. Information from geocoding tasks is visually displayed through a `PictureMarkerSymbol` graphic and a `Callout`. Suggestions are handled with a `SuggestListModel` whose `searchText` property is bound to the text property of the TextField; as the text property changes, the suggestions will automatically update.

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
