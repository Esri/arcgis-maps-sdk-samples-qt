# Raster rendering rule

Display a raster on a map and apply different rendering rules to that raster.

![](screenshot.png)

## Use case

Raster images whose individual pixels represent elevation values can be rendered in a number of different ways, including representation of slope, aspect, hillshade, and shaded relief. Applying these different rendering rules to the same raster allows for a powerful visual analysis of the data. For example, a geologist could interrogate the raster image to map subtle geological features on a landscape, which may become apparent only through comparing the raster when rendered using several different rules.

## How to use the sample

Run the sample and use the drop-down menu at the top to select a rendering rule.

## How it works
 When the sample starts, an `ImageServiceRaster` is created and added to a `RasterLayer`.  The `RasterLayer` is then added to the map as an operational layer.  Connect to the `loadStatusChanged` signal for the `ImageServiceRaster`. Once the `ImageServiceRaster` is loaded, the `RenderingRuleInfos` are fetched. Iterate over each item in the `RenderingRuleInfos` to get the rendering rule name and populate a data model using the names. This becomes the model for the combo box. When an item from the combo is selected and apply is pressed, the `RenderingRuleInfo` for the selected index is fetched from the service info. A `RenderingRule` object is created using the `RenderingRuleInfo` and applied to a newly created `ImageServiceRaster`. The `ImageServiceRaster` is then added to the `RasterLayer`.   

1.  Create an `ImageServiceRaster` using a URL to an online image service.
2.  After loading the raster, use `imageServiceRaster.serviceInfo.renderingRuleInfos` to get a list of `RenderingRuleInfo` supported by the service.
3.  Choose a rendering rule info to apply and use it to create a `RenderingRule`.
4.  Create a new `ImageServiceRaster` using the same URL.
5.  Apply the rendering rule to the new raster by setting `imageServiceRaster.renderingRule = renderingRule`.
6.  Create a `RasterLayer` from the raster for display.

## Relevant API

*   ImageServiceRaster
*   RasterLayer
*   RenderingRule

## About the data

This raster image service contains 9 LAS files covering Charlotte, North Carolina's downtown area. The lidar data was collected in 2007. Four Raster Rules are available for selection: None, RFTAspectColor, RFTHillshade, and RFTShadedReliefElevationColorRamp.

## Additional information

Image service rasters of any type can have rendering rules applied to them; they need not necessarily be elevation rasters. See the list of [raster function objects](https://developers.arcgis.com/documentation/common-data-types/raster-function-objects.htm) and syntax for rendering rules in the *ArcGIS REST API* documentation.

## Tags

raster, rendering rules, visualization