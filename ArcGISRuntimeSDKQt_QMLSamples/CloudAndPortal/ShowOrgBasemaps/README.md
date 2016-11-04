#Show organization basemaps

This sample demonstrates how to load a Portal and then use the organization's predefined basemaps query to get all basemaps. Thumbnails for these basemaps are then shown in a picker, to allow the user to select a basemap for use in a Map in a MapView.

![](screenshot.png)

##How it works
1. A `Portal` is constructed using a `Credential` of type OAuth.
2. When the App starts, the portal is loaded and the `AuthenticationManager` then issues a challenge for the supplied credential type.
3. The user is presented with an `AuthenticationView` which allows them to log-in
4. After a successful load, a request is made to `fetchBasemaps` from the portal instance.
5. When the basemaps are successfully retrieved, the portal's `BasemapListModel` is passed to a QML `ListView`.
6. A delegate shows each basemap's `title` and thumbnail image.
7. When the user double-clicks on a `Basemap` in the list, a `Map` is created using this (unloaded) item.
8. The map is then loaded and once the operation is complete it is set on a `MapView` to show the user's selected basemap

##Features
- Portal
- Credential
- AuthenticationManager
- BasemapListModel
- Map
- MapView
