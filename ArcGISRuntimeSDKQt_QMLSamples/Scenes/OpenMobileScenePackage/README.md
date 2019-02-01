# Open mobile scene package

Opens and displays a scene from a Mobile Scene Package (.mspk).

![](screenshot.png)

## Use Case
An .mspk file is an archive containing the data (specifically, basemaps and features), used to display an offline 3D scene.

## How it works
1. Create a `MobileScenePackage` using the path to the local `.mspk` file.
2. Use `MobileScenePackageUtility::isDirectReadSupported` to check whether the package can be read in the archived form (.mspk) or whether it needs to be unpacked.
3. If the mobile scene package requires unpacking, call `MobileScenePackageUtility::unpack` and wait for this to complete.
4. Call `MobileScenePackage::load` and check for any errors.
5. When the `MobileScenePackage` is loaded, obtain the first `Scene` from the `MobileScenePackage::scenes` list.
6. Create a `SceneView` and set the scene on the view for display.

## Relevant API
- MobileScenePackage
- MobileScenePackageUtility
- Scene
- SceneView

## Offline data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[San Francisco Streets TPK](https://www.arcgis.com/home/item.html?id=3f1bbf0ec70b409a975f5c91f363fe7d)| `<userhome>`/ArcGIS/Runtime/Data/tpk/SanFrancisco.tpk |

## Additional information
Before loading the MobileScenePackage, it is important to first check if direct read is supported. The MobileScenePackage could contain certain data types that would require the data to be unpacked. For example, Scenes containing raster data will need to be unpacked.

## Tags
Offline, Scene, MobileScenePackage

