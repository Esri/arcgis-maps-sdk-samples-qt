#Mobile map (search and route)

This sample demonstrates how to geocode and route using data in a map package. After selecting a map, if geocoding is available, click on the MapView to reverse geocode stops. After there are at least two geocoded stops, if routing is available, click on the route button to solve the route.

![](screenshot.png)

##How it works
A `QDir` is created using a dedicated folder, and `MobileMapPackage`s are created using the `.mmpk` files contained in that directory. When a `Map` is selected, geocoding is performed using its `locatorTask`, and a `RouteTask` is created using its `transportationNetworks`. The logic for the geocoding and routing is similar to the standalone "Offline geocode" and "Find a route" samples. The results are visually displayed with `GraphicsOverlay`s and a `Callout`. 

##Features
- Map
- MapView
- MobileMapPackage
- RouteTask
- RouteParameters
- RouteResult
- Stop
- LocatorTask
- GeocodeResult
- ReverseGeocodeParameters
- Graphic
- GraphicsOverlay
- PictureMarkerSymbol
- SimpleLineSymbol
- TextSymbol
- SimpleRenderer
- Callout

