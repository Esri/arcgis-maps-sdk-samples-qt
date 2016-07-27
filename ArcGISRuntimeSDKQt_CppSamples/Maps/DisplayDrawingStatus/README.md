#Display drawing status

This sample demonstrates how to use the `DrawStatus` value of the `MapView` to notify the user that the `MapView` is drawing.

![](screenshot.png)

##How it works
When the `MapView`'s `drawStatusChanged` signal is emitted, a boolean is set to true if the DrawStatus is `InProgress` and false if `Completed`. In QML, The `visible` property of a popup `Rectangle` is bound to that boolean so that it will display if the `MapView` is drawing. 

##Features
- MapView
- Map
- FeatureLayer
- ServiceFeatureTable
- Viewpoint
- Envelope

