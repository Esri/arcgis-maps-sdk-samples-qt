#Raster layer (file)

This sample demonstrates how to display local file based raster datasets with a RasterLayer.

![](screenshot.png)

##How it works
A `Raster` is created from the local file URL of a raster dataset. Next, this `Raster` is used to construct a new `RasterLayer`. Finally, the `RasterLayer` is added to a new `Basemap`, and the `Basemap` is added to a `Map`. Press the Add Raster button to navigate to a new raster dataset, and it will be added to the Map.

##Features
- Raster
- RasterLayer
- Basemap
- Map
- MapView

##Offline Data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[Colorado DEM](https://www.arcgis.com/home/item.html?id=8ad45e34ccd4412f8c2ad7629fb43a24)| `<userhome>`/ArcGIS/Runtime/Data/raster/Colorado.tif |
