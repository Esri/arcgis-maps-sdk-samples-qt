# Sync map and scene views

This sample demonstrates how to keep the viewpoints of multiple map or scene views in sync, so that navigating one view immediately updates the other.

![](screenshot.png)

## How to use the sample

Pan, zoom, and rotate the map or scene view. The other view will update automatically to match your navigation. Note that maps are 2D while scenes are 3D, so the results may not look identical, but the centers and scales will be kept the same.

## How it works

``Esri::ArcGISRuntime::MapView`` and ``Esri::ArcGISRuntime::SceneView`` have a signal ``viewpointChanged`` which is called each time the viewpoint updates. Inside the slot we get the viewpoint of the sender by calling ``currentViewpoint`` with ``ViewpointType::CenterAndScale``. We then pass that viewpoint into the ``setViewpoint`` of the other view, thus synchronizing all the views.

## Relevant API

- ``GeoView``
- ``GeoView::viewpointChanged``
- ``GeoView::isNavigating``
- ``GeoView::currentViewpoint``
- ``GeoView::setViewpoint``

## Tags

maps, scenes, viewpoints, synchronization
