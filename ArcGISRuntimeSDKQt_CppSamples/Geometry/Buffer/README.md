# Buffer
This sample demonstrates how to use the GeometryEngine::buffer to generate a polygon of a fixed distance around a point.

Creating buffers is a core concept in GIS as it allows for proximity analysis to find geographic features contained within a polygon. For example, suppose you wanted to know how many resturants are within a short walking distance of your home. The first step in this proximity analysis would be to generate a buffer polygon of a certain distance (say 1 mile) around your house.  

![Image](screenshot.png)

## How to use the sample
Tap on the map to create center map-points to generate buffer(s). You can optionally change the buffer distance (in miles) by adjusting the value in the text box before each tap on the map.

## How it works
The static method, `GeometryEngine.buffer`, is called by passing in a Point and a distance. The unit of measurement for the distances is automatically set to be the same as the input geometries, which in this case is meters. To provide a better user experience, the units entered in the User Interface (UI) for generating the buffer is in miles, so the provided number must be multiplied by 1609.34, which is the number of meters in a mile.

## Relevant API
 - GeometryEngine::buffer
 - GraphicsOverlay

## Tags
Analysis, Buffer, GeometryEngine
