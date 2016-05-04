#Find an address

This sample demonstrates how to geocode an address and show it on the map view. You can either type in an address or choose an address from the list to show it on the map view. The address location is marked with a pin. You can tap on the pin to show the address in a callout.

![](screenshot.png)

##How it works

The sample uses the `geocodeWithParameters` method on the `LocatorTask` to geocode an address. It returns a list of `GeocodeResult` objects in case of success. The first result is displayed on the map view using `GraphicsOverlay`. The attributes returned in the `GeocodeResult` object are used as title and detail fields for the callout.

##Features
- LocatorTask
- GeocodeParameters
- GeocodeResult
- GraphicsOverlay
- MapView
- Map
