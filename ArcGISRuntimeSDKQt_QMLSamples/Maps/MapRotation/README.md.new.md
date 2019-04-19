# Map rotation

This sample demonstrates how to rotate a map view. The rotation is set
using a slider control, with the rotation in degrees displayed next to
the slider.

![](screenshot.png)

## How it works

The slider is initialized to zero degrees and limited to values from 0
to 360. When the slider is released, it emits the `pressedChanged`
signal. The rotation is changed by calling the `setViewpoint` method on
the map view. Rotation using two fingers on a touch screen device is
disabled in this sample.

## Tips

This sample code includes a handler for the `pressedChanged` signal
rather than the `valueChanged` signal to avoid processing the many
signals emitted as the slider moves. Changing the rotation value rapidly
interrupts the animated rotation and may cause animation jitter.
Instead, you could defeat the animation so the rotation tracks the
slider value as it changes by using the method
`setViewpointWithAnimationCurve` with animation duration set to zero
seconds.

``` 
  onValueChanged: {
      var vp = mv.currentViewpoint(Enums.ViewpointTypeCenterAndScale);
      vp.setViewpointRotation(value);
      mv.setViewpointWithAnimationCurve(vp, 0.0, Enums.AnimationCurveLinear);
  }
```
