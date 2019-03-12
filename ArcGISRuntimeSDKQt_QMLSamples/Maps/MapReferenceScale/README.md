# Map reference scale
Set the map's reference scale and which feature layers should honor the reference scale.

![Map Reference Scale App](map-reference-scale.png)

## Use case

Setting a reference scale on an `ArcGISMap` fixes the size of symbols and text to the desired height and width at that scale. As you zoom in and out, symbols and text will increase or decrease in size accordingly. When no reference scale is set, symbol and text sizes remain the same size relative to the `MapView`.

Map annotations are typically only relevant at certain scales. For instance, annotations to a map showing a construction site are only relevant at that construction site's scale. So, when the map is zoomed out that information shouldn't scale with the `MapView`, but should instead remain scaled with the `ArcGISMap`. 

## How to use the sample

* Use the drop box at the top to set the map's reference scale (1:500,000 1:250,000 1:100,000 1:50,000).
* Click the button to set the map scale to the reference scale.
* Use the menu checkboxes in the layer menu to set which feature layers should honor the reference scale.

## How it works

1. Get and set the reference scale property on the `ArcGISMap` object.
1. Get and set the scale symbols property on each individual `FeatureLayer` object.

## Relevant API

* ArcGISMap
* FeatureLayer

## Additional Information

The map reference scale should normally be set by the map's author and not exposed to the end user like it is in this sample. 

#### Tags

Maps & Scenes




# Map reference scale

Set the map's reference scale and which feature layers should honor the reference scale.
This sample demonstrates ...       
This is **why** you would do it this way ...

![](screenshot.png)

## How to use the sample
e.g. Use the input controls to define a ... Click the "Go" button to ...

## How it works
e.g. In the `GeoView.Tapped` event, features in the `Map` are selected using an `Envelope` defined by the user's tap location ...

## Relevant API
 - ClassName1
 - MethodName

## Offline data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[San Francisco Streets TPK](https://www.arcgis.com/home/item.html?id=3f1bbf0ec70b409a975f5c91f363fe7d)| `<userhome>`/ArcGIS/Runtime/Data/tpk/SanFrancisco.tpk |

## Additional information
A standard level license is required to ...

## Tags
Routing, Network analysis, Geocode

