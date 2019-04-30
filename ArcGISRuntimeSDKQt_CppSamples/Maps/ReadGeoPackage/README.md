# Read a GeoPackage

Add rasters and feature tables from a GeoPackage to a map.

![](screenshot.png)

## Use case
The OGC GeoPackage specification defines an open standard for sharing raster and vector data. You may want to use GeoPackage files to support file-based sharing of geographic data.

## How to use the sample
When the sample loads, the feature tables and rasters from the GeoPackage will be shown on the map.

## How it works
1. Open the GeoPackage using GeoPackage::load
2. Iterate through available rasters, exposed by GeoPackage::geoPackageRasters.
3. For each raster, create a raster layer using the raster in the list, then add it to the map.
4. Iterate through available feature tables, exposed by GeoPackage::geoPackageFeatureTables.
5. For each feature table, create a feature layer using the feature table in the list, then add it to the map.

## Relevant API
- GeoPackage
- GeoPackageRaster
- GeoPackage::geoPackageRasters
- GeoPackageFeatureTable
- GeoPackage::geoPackageFeatureTables

## About the data
This sample features a GeoPackage with datasets that cover Aurora, Colorado: Public art (points), Bike trails (lines), Subdivisions (polygons), Airport noise (raster), and liquor license density (raster).

## Additional Information
GeoPackage uses a single SQLite file (.gpkg) that conforms to the OGC GeoPackage Standard. You can create a GeoPackage file (.gpkg) from your own data using the create a SQLite Database tool in ArcGIS Pro.

## Offline data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[AuroraCO.gpkg](https://www.arcgis.com/home/item.html?id=68ec42517cdd439e81b036210483e8e7)| `<userhome>`/ArcGIS/Runtime/Data/gpkg/AuroraCO.gpkg |

## Tags
GeoPackage, Maps, Rasters, Layers, Tables, OGC, package, container
