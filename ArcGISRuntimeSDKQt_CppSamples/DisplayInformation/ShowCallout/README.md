#Show callout

This sample demonstrates how to show location coordinates on a `Map` using a `Callout`.

![](screenshot.png)

##How it works
When `MapView`'s `mouseClick` signal is emitted, the `QMouseEvent` position is translated to coordinates and set to the `detail` property of a the MapView `calloutData`. In QML, a Callout is created, and its `calloutData` property is bound to the MapView's calloutData.

##Features
- Map
- MapView
- Callout
- Viewpoint
- SpatialReference
