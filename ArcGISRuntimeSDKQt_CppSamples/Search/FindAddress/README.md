# Find address

Find the location for an address.

![](screenshot.png)

## Use case

A user can input a raw address into your app's search bar and zoom to the address location.

## How to use the sample

For simplicity, the sample comes loaded with a set of suggested addresses. Choose an address from the suggestions or submit your own address to show its location on the map in a callout.

## How it works

1. Create a `LocatorTask` using the URL to a locator service.
2. Set the `GeocodeParameters` for the locator task and specify the geocode's attributes.
3. Get the matching results from the `GeocodeResult` using `locatorTask::geocodeWithParametersAsync(addressString, geocodeParameters)`.
4. Create a `Graphic` with the geocode result's location and store the geocode result's attributes in the graphic's attributes.
5. Show the graphic in a `GraphicsOverlay`.

## Relevant API

* GeocodeParameters
* GeocodeResult
* LocatorTask

## About the data

This sample uses the [World Geocoding Service](https://www.arcgis.com/home/item.html?id=305f2e55e67f4389bef269669fc2e284).

## Tags

address, geocode, locator, search
