# Offline geocode

Geocode addresses to locations and reverse geocode locations to addresses offline.

![](screenshot.png)

## Use case

You can use an address locator file to geocode addresses and locations. For example, you could provide offline geocoding capabilities to field workers repairing critical infrastructure in a disaster when network availability is limited.

## How to use the sample

Type the address in the Search menu option or select from the list to `Geocode` the address and view the result on the map. Tap the location you want to reverse geocode. Select the pin to highlight the `PictureMarkerSymbol` (i.e. single tap on the pin) and then tap-hold and drag on the map to get real-time geocoding.

## How it works

1. Use the path of a .loc file to create a `LocatorTask` object.
2. Set up `GeocodeParameters` and call `geocode` to get geocode results.

## Relevant API

* GeocodeParameters
* GeocodeResult
* LocatorTask
* ReverseGeocodeParameters

## Offline Data

Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples#use-offline-data-in-the-samples).

Link | Local Location
---------|-------|
|[SanDiego tpk File](https://www.arcgis.com/home/item.html?id=7277419732964266b909bf2bae1ddb54)| `<userhome>`/ArcGIS/Runtime/Data/tpk/streetmap_SD.tpk |
|[SanDiego loc Files](https://www.arcgis.com/home/item.html?id=c88c170e18f74740a0e4c660a5ff51c4)| `<userhome>`/ArcGIS/Runtime/Data/Locators/SanDiegoStreetAddress/SanDiego_StreetAddress.loc |

## Tags

geocode, geocoder, locator, offline, package, query, search
