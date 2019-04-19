# Identify graphics

This sample demonstrates how to identify graphics in a graphics overlay.

![](screenshot.png)

## How it works

When you select a graphic on the map, a message box appears. This is
accomplished by first setting up a signal handler for the `mouseClicked`
signal on the `MapView`. Once this signal is emitted, call
`identifyGraphicsOverlay` on the `MapView`, and pass in the
`GraphicsOverlay` to identify on, the mouse coordinates, the tolerance,
and the max features. Finally, set up a signal handler on the `MapView`
to handle the `identifyGraphicsOverlayStatusChanged` signal. Once the
signal emits and the `identifyGraphicsOverlayStatus` is completed, the
`identifyGraphicsOverlayResults` will be hydrated with any of the
graphics that were identified. If the length of this list of graphics is
greater than 0, show the message dialog that indicates a graphic has
been identified.
