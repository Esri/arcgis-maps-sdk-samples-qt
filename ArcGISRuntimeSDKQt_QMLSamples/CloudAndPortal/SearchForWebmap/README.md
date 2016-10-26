#Search for webmap by keyword

This sample demonstrates how find portal items by using a keyword, and limit the results to items of a certain type - in this case, webmaps.

![](screenshot.png)

##How it works
1. A `Portal` is created up with a `Credential` using `OAuthModeUser`.
2. When the app starts, the portal is loaded and the user is prompted to login.
3. A search bar allows the user to enter a keyword (or tag) to search for - e.g. "usa". NOTE if multiple item types are required these can be set via the `types` property.
4. The search term is bound to the searchString for a `PortalQueryParametersForItems` which also has the property `type: Enums.PortalItemTypeWebMap`
5. The query parameters are passed to the portal's `findItems` method.
6. Once the findItems Task is complete, a `PortalQueryResultSetForItems` is obtained from the portal's `findItemsResult` property.
7. The `PortalItemListModel` from the itemsResult is set on a QML ListView to display the list of webmaps.
8. When the user double-clicks on a webmap in the list, the `PortalItem` is loaded anad set as a `Map` on the `MapView`

##Features
- Portal
- PortalQueryParametersForItems
- PortalQueryResultSetForItems
- PortalItemListModel
- PortalItem
