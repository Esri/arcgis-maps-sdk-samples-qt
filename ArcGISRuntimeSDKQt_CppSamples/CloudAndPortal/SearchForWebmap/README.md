#Search for web map by keyword

This sample demonstrates how find portal items by using a keyword, and limit the results to items of a certain type - in this case, web maps.

![](screenshot.png)

##How it works
1. A `Portal` is created with a `Credential` using `OAuthModeUser`.
2. When the app starts, the portal is loaded and the user is prompted to login.
3. A search bar allows the user to enter a keyword (a tag) to search for - e.g. "usa". 
4. The search term is bound to the searchString for a `PortalQueryParametersForItems` which also has the property `type: Enums.PortalItemTypeWebMap`. NOTE if multiple item types are required these can be set via the `types` property.
5. The query parameters are passed to the portal's `findItems` method.
6. Once the `findItems` task is complete, a `PortalQueryResultSetForItems` is obtained from the portal's `findItemsResult` property.
7. The `PortalItemListModel` from the items result is set on a `ListView` to display the set of web maps. If many web maps match the search criteria, the results will contain only the 1st set (to allow "paging" through the results in batches).
8. If there are additional results, a subsequent query can be issued by clicking the "More Results" button. Ths passes the itemResult's `nextQueryParameters` property to a new `findItems` operation.
9. When the user double-clicks on a web map in the list, the `PortalItem` is loaded and set as a `Map` on the `MapView`

##Features
- Portal
- PortalQueryParametersForItems
- PortalQueryResultSetForItems
- PortalItemListModel
- PortalItem