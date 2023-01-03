# Display feature layers

Display feature layers from various data sources.

![](screenshot.png)

## Use case

Feature layers, like all layers, are visual representations of data and are used on a map or scene. In the case of feature layers, the underlying data is held in a feature table or feature service.

Feature services are useful for sharing vector GIS data with clients so that individual features can be queried, displayed, and edited. There are various online and offline methods to load feature services.

## How to use the sample

Use the drop down to display different feature layers on the map. Pan and zoom the map to view the feature layers.

## How it works

1. Set the basemap with a `BasemapStyle` enum
2. Create a feature layer with a `Geodatabase`
    - Create a `FeatureLayer` component
    - Add a `Geodatabase` component and provide the path to the .geodatabase
3. Create a feature layer with a `GeoPackage`
    - Create a `FeatureLayer` component
    - Create a `GeoPackage` component and provide the path to the .gpkg
    - Load the GeoPackage
    - Set the feature layer's feature table to a `GeoPackageFeatureTable` from the GeoPackage
4. Create a feature layer with a `PortalItem`
    - Create a `FeatureLayer` component
    - Add a `PortalItem` component and provide the itemID property
5. Create a feature layer with a `ServiceFeatureTable`
    - Create a `FeatureLayer` component
    - Set the featureTable property to a `ServiceFeatureTable` component with a url
6. Create a feature layer with a shapefile
    - Create a `FeatureLayer` component
    - Set the featureTable property to a `ShapefileFeatureTable` component with the path to the .shp
7. Add the created feature layer to the `Map`'s `operationalLayers`
    

## Relevant API

* FeatureLayer
* Geodatabase
* GeoPackage
* PortalItem
* ServiceFeatureTable
* ShapefileFeatureTable

## Offline data

Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

| Link | Local Location |
| --- | --- |
| [Los Angeles Trailheads geodatabase](https://www.arcgis.com/home/item.html?id=cb1b20748a9f4d128dad8a87244e3e37) | `<userhome>`/ArcGIS/Runtime/Data/geodatabase/LA_Trails.geodatabase|
| [Aurora, Colorado GeoPackage](https://www.arcgis.com/home/item.html?id=68ec42517cdd439e81b036210483e8e7) | `<userhome>`/ArcGIS/Runtime/Data/gpkg/AuroraCO.gpkg|
| [Scottish Wildlife Trust Reserves Shapefile](https://www.arcgis.com/home/item.html?id=15a7cbd3af1e47cfa5d2c6b93dc44fc2) | `<userhome>`/ArcGIS/Runtime/Data/shp/ScottishWildlifeTrust_ReserveBoundaries_20201102.shp|


## About the data

This sample uses the [Northern Los Angeles County Geology Service](https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/Geology/FeatureServer/9), [Trees of Portland portal item](https://www.arcgis.com/home/item.html?id=1759fd3e8a324358a0c58d9a687a8578), [Los Angeles Trailheads geodatabase](https://www.arcgis.com/home/item.html?id=2b0f9e17105847809dfeb04e3cad69e0), [Aurora, Colorado GeoPackage](https://www.arcgis.com/home/item.html?id=68ec42517cdd439e81b036210483e8e7), and [Scottish Wildlife Trust Reserves Shapefile](https://www.arcgis.com/home/item.html?id=15a7cbd3af1e47cfa5d2c6b93dc44fc2). 

The Scottish Wildlife Trust shapefile data is provided from Scottish Wildlife Trust under [CC-BY licence](https://creativecommons.org/licenses/by/4.0/). Data © Scottish Wildlife Trust (2022).


## Tags

feature, geodatabase, geopackage, layers, service, shapefile, table

