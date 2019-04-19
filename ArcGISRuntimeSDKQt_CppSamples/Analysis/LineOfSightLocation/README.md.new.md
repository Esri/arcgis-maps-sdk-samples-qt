# Line of Sight (location)

This sample demonstrates how to perform a line of sight analysis between
two points in a SceneView.

![](screenshot.png)

## How it works

A `LineOfSight` analysis is a type of visual analysis you can perform on
a scene. The `LineOfSight` analysis aims to answer the question ‘What
are the visible and obstructed portions of a line between two
locations?’. The output is a line in an overlay with two different
colors - one representing the visible areas and the other representing
the obstructed areas.

This is achieved by first creating a `LocationLineOfSight`, and
assigning values for the `observerLocation` and `targetLocation`. Once
the `LineOfSight` is created, it is added to an AnalysisOverlay, which
is then added to the SceneView. Each time the screen is tapped, the
LineOfSight’s `targetLocation` is updated with the clicked point, and
the `LineOfSight` is automatically recalculated and displayed in the
overlay.
