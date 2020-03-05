# Show callout

Show a callout with the latitude and longitude of user-tapped points.

![](screenshot.png)

## Use case

Callouts are used to display temporary detail content on a map. You can display text and arbitrary UI controls in callouts.

## How to use the sample

Tap anywhere on the map. A callout showing the WGS84 coordinates for the tapped point will appear.

## How it works

1. Listen for `mouseClicked` signal on the map view.
2. When the user taps, get the tapped location using the mouseEvent x and y coordinates, `MapQuickView::screenToLocation(mouseEvent.x(), mouseEvent.y())`.
3. Create a string to display the coordinates; note that latitude and longitude in WGS84 map to the Y and X coordinates.
4. Create a new callout definition using a title and the coordinate string.
5. Display the callout by calling `setVisible` and `setLocation` on the `CalloutData`.

## Relevant API

* CalloutDefinition
* GeometryEngine::project
* GeoViewTappedEventArgs
* MapView::geoViewTapped
* MapView::showCalloutAt

## Tags

balloon, bubble, callout, flyout, flyover, info window, popup, tap
