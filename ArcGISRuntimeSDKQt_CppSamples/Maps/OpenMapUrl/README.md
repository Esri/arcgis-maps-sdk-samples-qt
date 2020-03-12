# Open Map (URL)

Display a web map.

![](screenshot.png)

## Use case

For displaying web maps stored on ArcGIS Online (e.g. soil survey results, as demonstrated in this sample).

## How to use the sample

A web map can be selected from the drop-down list. On selection the web map displays in the map view.

## How it works

1. Create a `Portal`.
2. Create a `PortalItem` using the Portal and the web map ID: `new PortalItem(portal, ID)`.
3. Create a `Map` using the portal item.
4. Set the map to the `MapView`.

## Relevant API

* Map
* MapView
* Portal
* PortalItem

## Additional information

Mobile map packages can also include address locators and geometric networks. For an example of this, see the "Mobile Map Search and Route Sample".

## Tags

portal item, web map
