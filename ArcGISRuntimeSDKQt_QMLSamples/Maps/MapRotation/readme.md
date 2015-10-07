#Map rotation

![](screenshot.png) 

##

This sample demonstrates how to rotate a map. The rotation is set using a slider control, with the rotation in degrees displayed to the right of the slider. Rotation using two fingers on a touch screen device is diabled in this sample.

## How it works

The user chooses the number of degrees to rotate using a slider control at the bottom of the display. The slider is initialized to zero degrees and limited to values from 0 to 360. As the slider is moved, its value changes, emitting a signal that is caught by the slot named `onPressedChanged()`. This simple slot sets the rotation of the map view to the value of the slider.

The value displayed in the text box to the right of the slider is bound to the value of the slider. 

## Features

- MapView
- Map
- BasemapStreets
- ViewpointCenter
- Slider (Qt Quick control)

