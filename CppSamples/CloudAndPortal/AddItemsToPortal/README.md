# Add items to portal

This sample demonstrates how to add and delete items in a user's portal.

![](screenshot.png)

## Use case

Portals allow you to share and publish data with others. For example, you may create or collect some data on your device and want to upload that to a cloud storage location to share with others or simply save for later use. In this case, a CSV file is created and uploaded to ArcGIS Online.

## How to use the sample

1. Press the Authenticate Portal button and sign into your ArcGIS Online account.
2. Press the Add Item button, and this will add the CSV to your `Portal`.
3. Press the Delete Item button, and this will delete that same item from your `Portal`.

## How it works

1. A `Portal` is created and an `OAuthUserConfiguration` is created with the portal URL, a client Id and a redirect URI corresponding to the portal.
2. The `OAuthUserConfiguration` is added to the Toolkit's `OAuthUserConfigurationManager` in order to opt-in to receiving OAuth challenges.
3. The Portal is loaded and displays an OAuth web-based login page to allow user credentials to be entered.
4. Once the sign-in process is complete, a `PortalItem` of type `CSV` is created using this `Portal` instance.
5. When the portal is successfully loaded, the `PortalUser` object is obtained from the `Portal`.
6. When the "Add Item" button is clicked, the item is added via the `PortalUser`'s `addPortalItemWithUrlAsync` method. The `PortalItem` and the local URL of the CSV file are supplied as parameters.
7. When the item has been successfully added, it is loaded in order to retrieve online properties such as the `itemId`.'
8. When the "Delete Item" button is clicked, the item is passed as a parameter for the portalUser's `deleteItemAsync` method.

## Relevant API

* Authenticator
* Authentication::OAuthUserConfiguration
* OAuthUserConfigurationManager
* Portal
* PortalItem
* PortalUser::addPortalItemWithUrlAsync
* PortalUser::deleteItemAsync

## Additional information

This sample uses the `OAuthUserConfigurationManager` toolkit component and requires the [toolkit](https://github.com/Esri/arcgis-maps-sdk-toolkit-qt) to be cloned and set-up locally.

## Tags

add item, cloud, portal
