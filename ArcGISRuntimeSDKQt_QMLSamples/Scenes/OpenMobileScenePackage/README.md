# Open mobile scene package

Opens and displays a scene from a Mobile Scene Package (.mspk).

![](screenshot.png)

## Use Case

An .mspk file is an archive containing the data (specifically, basemaps and features), used to display an offline 3D scene.

## How to use the sample

When the sample opens, it will automatically display the Scene in the Mobile Map Package.

Since this sample works with a local .mspk, you may need to download the file to your device.

## How it works

This sample takes a Mobile Scene Package that was created in ArcGIS Pro, and displays a `Scene` from within the package in a `SceneView`.

1. Create a `MobileScenePackage` using the path to the local `.mspk` file.
2. Call `MobileScenePackage.load` and check for any errors.
3. When the `MobileScenePackage` is loaded, obtain the first `Scene` from the `MobileScenePackage.scenes` list.
4. Create a `SceneView` and set the scene on the view for display.

## Relevant API

* MobileScenePackage
* SceneView

## Offline data

Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples#use-offline-data-in-the-samples).

Link | Local Location
---------|-------|
|[Philadelphia MSPK](https://www.arcgis.com/home/item.html?id=7dd2f97bb007466ea939160d0de96a9d)| `<userhome>`/ArcGIS/Runtime/Data/mspk/philadelphia.mspk |

## Additional information

Before loading the MobileScenePackage, it is important to first check if direct read is supported. The MobileScenePackage could contain certain data types that would require the data to be unpacked. For example, Scenes containing raster data will need to be unpacked.

## Tags

offline, scene
