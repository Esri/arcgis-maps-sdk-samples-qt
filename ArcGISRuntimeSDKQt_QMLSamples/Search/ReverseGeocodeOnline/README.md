# Reverse geocode

This sample demonstrates how to get the address for a point on the map.

![](screenshot.png)

## How to use the sample
Click on a point on the map to perform online reverse geocoding and show the matching results on the map.

## How it works
To perform online reverse geocode:
1. Create the `Map` with `Basemap`.
2. Create a `LocatorTask` using a URL.
3. Set the `ReverseGeocodeParameters` for the `LocatorTask` and specify the geocode's attributes.
4. Get the matching results from the `GeocodeResult` using `ReverseGeocodeWithParameters`
5. Change the attributes of the `MapView`'s `CalloutData` and display the location using a `Callout`

## Relevant API
 - Callout
 - CalloutData
 - GraphicsOverlay
 - LocatorTask
 - PictureMarkerSymbol
 - ReverseGeocodeParameters

## Offline data
Link | Local Location
---------|-------|
[pin PNG file](https://github.com/Esri/arcgis-runtime-samples-java/blob/master/search/reverse-geocode-online/src/main/resources/pin.png)| `<userhome>`/ArcGIS/Runtime/Data/symbol/pin.png |


## Tags
Callout, Geocode