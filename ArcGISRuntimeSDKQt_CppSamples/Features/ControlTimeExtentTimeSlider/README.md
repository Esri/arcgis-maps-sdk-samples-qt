# ControlTimeExtentTimeSlider

This sample demonstrates how to use the time slider to visualize temporal data by applying a specific time extent.
When viewing feature layers that contain a large amount of data with timestamps, you may want to filter the data to only show data relevant to a specific time range. This can help visualize changes in the data over time and aids in making better interpretations and predictions of future trends.


![](screenshot.png)

## How to use the sample

Use the slider at the bottom of the map to customize the date range for which you want to view the data. Once the start and end date have been selected, the mapview will update to only show the relevant data that falls in the time extent selected. 
Use the play button to step through the data 4 days at a time. Use the previous and next buttons to manually step through the data in 4 day increments.

## How it works
A map is created and then the image layer that contains the temporal data is added to the list of operational layers of the map. Using the toolkit, add the time slider widget to the map to navigate through the data.

## Relevant API
- ArcGISMapImageLayer
- Map
- MapView

## Tags
Animate, Data, Filter, Time, Time extent, Time frame, Toolkit
