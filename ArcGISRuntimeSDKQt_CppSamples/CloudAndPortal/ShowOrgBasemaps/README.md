#Show organization basemaps

This sample demonstrates how to load a Portal and then use the organization's predefined basemaps query to get all basemaps. Thumbnails for these basemaps are then shown in a picker, to allow the user to select a basemap for use in a Map in a MapView.

![](screenshot.png)

##How it works
1. The user is prompted to load a portal anonymously or with a log-in.
2. A `Portal` is then loaded - if the user chose to authenticate this uses a `Credential` of type OAuth.
3. When the app starts, the portal is loaded and if required, the `AuthenticationManager` issues a challenge for the supplied credential type.
4. The user is presented with an `AuthenticationView` which allows them to log-in
5. After a successful load, a request is made to `fetchBasemaps` from the portal instance.
6. When the basemaps are successfully retrieved, the portal's `BasemapListModel` is passed to a QML `GridView`.
7. A delegate shows each basemap's `title` and thumbnail image.
8. When the user double-clicks on a `Basemap` in the list, a `Map` is created using this (unloaded) item.
9. The map is then loaded and once the operation is complete it is set on a `MapView` to show the user's selected basemap
10. A back button allows the user to return to the list of basemaps.

##Features
- Portal
- Credential
- AuthenticationManager
- BasemapListModel
- Map
- MapView
