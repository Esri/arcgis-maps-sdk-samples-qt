# Picture marker symbol

Use pictures for markers.

![](screenshot.png)

## Use case

When marking geoelements on a map, using custom, unique symbols can be helpful for highlighting and differentiating between locations. For example, a tourism office may use pictures of landmarks as symbols on an online map or app, to help prospective visitors to orient themselves more easily around a city.

## How to use the sample

When launched, this sample displays a map with three picture marker symbols. Pan and zoom to explore the map.

## How it works

1. Create a `PictureMarkerSymbol` using the URL to an online or local image.
2. Create a `Graphic` and set its symbol to the picture marker symbol.

## Relevant API

* PictureMarkerSymbol

## Offline Data
To set up the sample's offline data, see the [Use offline data in the samples](https://github.com/Esri/arcgis-runtime-samples-qt#use-offline-data-in-the-samples) section of the Qt Samples repository overview.

Link | Local Location
---------|-------|
|[orange_symbol Png File](https://www.arcgis.com/home/item.html?id=1c95ea3b6e4843cdbd6ae354efb97f0c)| `<userhome>`/ArcGIS/Runtime/Data/symbol/orange_symbol.png |

## About the data

The picture marker symbols in this sample are all constructed from different types of resources:
 - [Campsite symbol constructed from a URL](http://sampleserver6.arcgisonline.com/arcgis/rest/services/Recreation/FeatureServer/0/images/e82f744ebb069bb35b234b3fea46deae)
 - Blue pin with a star stored in the resource folder that comes with the application
 - Orange pin created from a file path on disk (which is written to disk when the app starts and cleaned up when the app closes).

## Tags

graphics, marker, picture, symbol, visualization
