# Create mobile geodatabase

This sample demonstrates how to create and share a mobile geodatabase.

![](screenshot.png)

## Use case

A mobile geodatabase is a collection of various types of GIS datasets contained in a single file (.geodatabase) on disk that can store, query, and manage spatial and nonspatial data. Mobile geodatabases are stored in a SQLite database and can contain up to 2 TB of portable data. Users can create, edit and share mobile geodatabases across ArcGIS Pro, ArcGIS Runtime, or any SQL software. These mobile geodatabases support both viewing and editing and enable new offline editing workflows that don’t require a feature service.

For example, a user would like to track the location of their device at various intervals to generate a heat map of the most visited locations. The user can add each location as a feature to a table and generate a mobile geodatabase. The user can then instantly share the mobile geodatabase to ArcGIS Pro to generate a heat map using the recorded locations stored as a geodatabase feature table.

## How to use the sample

Select "Create new .geodatabase" to create a new mobile geodatabase in a temporary directory. Click or tap on the map to add features to the mobile geodatabase. Select "View table" to view the contents of the geodatabase feature table. Select "Clear features" to remove all features from the geodatabase feature table. Select "Close .geodatabase" to save and close the geodatabase so it is ready to be hypothetically shared or uploaded to ArcGIS Online.

## How it works

1. Create a new `Geodatabase` at a given path that does not already exist with the `Geodatabase::instance()` singleton.
2. Create a `TableDescription` and add a list of `FieldDescription`s to the table description.
3. Create a `GeodatabaseFeatureTable` from the geodatabase with the `TableDescription` using `Geodatabase::createTable(TableDescription *tableDescription)`.
4. Create an `ArcGISFeature` on a selected map point using `FeatureTable::createFeature(const QVariantMap &attributes, const Geometry &geometry, QObject *parent = nullptr)`.
5. Add the feature to the table using `FeatureTable::addFeature(Feature *feature)`
6. Each feature added to the feature table is committed to the mobile geodatabase file.
7. Close the mobile geodatabase to safely share the ".geodatabase" file using `Geodatabase::close()`

## Relevant API

* ArcGISFeature
* FeatureLayer
* FeatureTable
* FieldDescription
* Geodatabase
* GeodatabaseFeatureTable
* TableDescription

## Additional information

Learn more about mobile geodatabases and how to utilize them on the [ArcGIS Pro documentation](https://pro.arcgis.com/en/pro-app/latest/help/data/geodatabases/manage-mobile-gdb/mobile-geodatabases.htm) page. The following mobile geodatabase behaviors are supported in ArcGIS Runtime: annotation, attachments, attribute rules, contingent values, dimensions, domains, feature-linked annotation, subtypes, utility network and relationship classes.

Learn more about the types of fields supported with mobile geodatabases on the [ArcGIS Pro documentation](https://pro.arcgis.com/en/pro-app/latest/help/data/geodatabases/overview/arcgis-field-data-types.htm) page.

## Tags

arcgis pro, database, feature, feature table, geodatabase, mobile geodatabase, sqlite
