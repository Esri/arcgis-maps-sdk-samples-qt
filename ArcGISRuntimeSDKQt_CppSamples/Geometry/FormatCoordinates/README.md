# Format coordinates (URL)

Coordinates can be written as text that is formatted in different ways. For example, latitude-longitude coordinates can be formatted as decimal degrees, or as degrees, minutes, and seconds. This sample demonstrates how to convert a map location `Point` to a number of different coordinate notations (decimal degrees; degrees, minutes, seconds; Universal Transverse Mercator (UTM), and United States National Grid (USNG)), by using `CoordinateFormatter`. 

The `CoordinateFormatter` also supports Military Grid Reference System (MGRS), Global Area Reference System (GARS), and World Geographic Reference System (GEOREF) notations, using similar methods to those shown in this sample app.

Click or tap on the map to indicate a location. The location will be formatted into each of the four formats demonstrated. Additionally, coordinate notation strings can be converted to a `Point` and shown in the map by clicking or tapping on the notation values shown and entering a coordinate in the appropriate notation.

![](screenshot.png)

## How it works

Changing the text for any of the coordinate strings calls an invokable C++ function 
that attempts to convert the changed string to a `Point`. If successful, the `Point`
is used to update all the coordinate strings and the geometry of the `Graphic`. 

When you click or tap on the `MapView`, the location is converted from screen coordinates
to a `Point` in map coordinates. As in the previous case, the `Point`
is used to update all the coordinate strings and the geometry of the `Graphic`. 

## Features
- CoordinateFormatter
- Point
- Graphic
