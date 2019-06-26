# Integrated windows authentication

Uses Windows credentials to access services hosted on a portal secured with Integrated Windows Authentication (IWA).

![](screenshot.png)

## Use case

IWA, which is built into Microsoft Internet Information Server (IIS), works well for intranet applications, but isn't always practical for internet apps.

## How to use the sample

Enter the URL to your IWA-secured portal. Click either the "Search Public" (which will search for portals on www.arcgis.com) or "Search Secure" (which will search your IWA-secured portal), for web maps stored on the portal. You will be prompted for a user name, password, and domain (some platforms will use the current Windows login). If you authenticate successfully, portal item results will display in the list. Select a web map item to display it in the map view.

## How it works

1. The `AuthenticationManager` object is configured with a challenge handler that will prompt for a Windows login (username, password, and domain) if a secure resource is encountered.
2. When a search for portal items is performed against an IWA-secured portal, the challenge handler creates an `ArcGISNetworkCredential` object from the information entered by the user.
3. If the user authenticates, the search returns a list of web maps (`ArcGISPortalItem`) and the user can select one to display as a `Map`.
4. On some platforms, the current Windows account is used by default and a login prompt will not be shown if it can authenticate successfully.

## Relevant API

* `AuthenticationManager`
* `ArcGISPortal`
* `ArcGISNetworkCredential`

## About the data

This sample searches for web map portal items on a secure portal. To successfully run the sample, you need:
 - Access to a portal secured with Integrated Windows Authentication that contains one or more web map items.
 - A login that grants you access to the portal.

## Tags

Authentication, Security, Windows, Portal
