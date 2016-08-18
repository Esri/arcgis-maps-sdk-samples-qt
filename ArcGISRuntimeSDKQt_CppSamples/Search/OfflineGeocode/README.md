#Offline geocode

This sample demonstrates how to geocode and reverse geocode addresses and locations without network connectivity.

![](screenshot.png)

##How it works
An `ArcGISTiledLayer` and `LocatorTask` are created using local files. Geocoding is performed with inputs from a QtQuick `TextField`. Reverse geocoding is handled through various mouse events. Information from geocoding tasks is visually displayed through a `PictureMarkerSymbol` graphic and a `Callout`. Suggestions are handled through the `SuggestListModel`'s `setSearchText` function; the TextField's text property is used as input for the function. When the searchText is set, the SuggestListModel will automatically update.

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
