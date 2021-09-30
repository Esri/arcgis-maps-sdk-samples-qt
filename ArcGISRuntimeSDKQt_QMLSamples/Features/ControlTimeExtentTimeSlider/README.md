# Control time extent using toolkit time slider

This sample demonstrates how to use the time slider from the toolkit to visualize temporal data by applying a specific time extent.
When viewing feature layers that contain a large amount of data with timestamps, you may want to filter the data to only show data relevant to a specific time range. This can help visualize changes in the data over time and aid in making better interpretations and predictions of future trends.


![](screenshot.png)

## How to use the sample

Use the slider at the bottom of the map to customize the date range for which you want to view the data. Once the start and end date have been selected, the mapview will update to only show the relevant data that falls in the time extent selected. 
Use the play button to step through the data 4 days at a time. Use the previous and next buttons to manually step through the data in 4 day increments.

## How it works
A map is created and then the ArcGIS image layer that contains time-enabled data is added to map's list of operational layers. Then a time slider is created from the ArcGIS Runtime Toolkit to allow users to show data only from the given date range. This sets up all the necessary calls to visualize and step through the data.

## Relevant API
- ArcGISMapImageLayer
- Map
- MapView

## Additional information

This sample uses [Atlantic hurricane data](https://sampleserver6.arcgisonline.com/arcgis/rest/services/Hurricanes/MapServer) from the National Hurricane Center (NOAA / National Weather Service) and the `TimeSlider` toolkit component which requires the [toolkit](https://github.com/Esri/arcgis-runtime-toolkit-qt) to be cloned and set up locally. For information about setting up the toolkit, see the repository's root README.md [here](https://github.com/Esri/arcgis-runtime-toolkit-qt/blob/main/uitools/README.md).

## Tags
animate, data, filter, time, time extent, time frame, toolkit
