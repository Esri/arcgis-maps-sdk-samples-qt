# Add items to portal

This sample demonstrates how to add and delete items (in this case a local .csv file) in a user's portal.

![](screenshot.png)

## Use case

Portals allow you to share and publish data with others. For example, you may create or collect some data on your device and want to upload that to a cloud storage location to share with others or simply save for later use. In this case, a CSV file is created and uploaded to ArcGIS Online.

## How to use the sample

1. Press the Authenticate Portal button and sign into your ArcGIS Online account.
2. Press the Add Item button, and this will add the CSV to your `Portal`.
3. Press the Delete Item button, and this will delete that same item from your `Portal`.

## How it works
1. A `Portal` is created with a `Credential` using `OAuthModeUser`.
2. A `PortalItem` of type CSV is created using this portal.
3. When the "Autheticate" button is clicked, the portal is loaded and the `AuthenticationView` is displayed to request credentials.
4. When the portal is successfully loaded, the `PortalUser` object is obtained from the portal.
5. When the "Add Item" button is clicked, the item is added via the portalUser's `addPortalItemWithUrl` method. The portalItem and the local Url of the CSV file are supplied as parameters.
6. When the item has been succesfully added, it is loaded in order to retrieve online properties such as the `itemId`.'
7. When the "Delete Item" button is clicked, the item is passed as a parameter for the portalUser's `deleteItem` method. 

## Relevant API

* AuthenticationView
* Portal
* PortalItem
* PortalUser
* PortalUser.addPortalItemWithUrl
* PortalUser.deleteItem

## Tags

add item, cloud, portal
