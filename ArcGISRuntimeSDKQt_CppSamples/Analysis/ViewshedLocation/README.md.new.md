# Viewshed (location)

This sample demonstrates how to calculate a viewshed from a Point
location.

![](screenshot.png)

## How it works

A `Viewshed` analysis is a type of visual analysis you can perform on a
scene. The viewshed aims to answer the question ‘What can I see from a
given location?’. The output is an overlay with two different colors -
one representing the visible areas an the other representing the
obstructed areas.

This is achieved by first creating a `LocationViewshed`, and assigning
values for the location, heading, pitch, and various other properties.
Once the viewshed is created, it is added to an `AnalysisOverlay`, which
is then added to the `SceneView`. Each time the screen is tapped, the
viewshed’s `location` is updated with the clicked point, and the
viewshed is automatically recalculated and displayed in the overlay.
Several QML controls are available in an options panel, which allow you
to interactively change the pitch, heading, visibility, and so on.
