# Integrated windows authentication

Uses Windows credentials to access services hosted on a portal secured with Integrated Windows Authentication (IWA).

![](screenshot.png)

## Use case

IWA, which is built into Microsoft Internet Information Server (IIS), works well for intranet applications, but isn't always practical for internet apps.

## How to use the sample

Enter the URL to your IWA-secured portal in the text field at the top of the screen. Select either the "Search Public" (which will search for portals on www.arcgis.com) or "Search Secure" (which will search your IWA-secured portal), for web maps stored on the portal. If you tap "Search Secure", you will be prompted for a username (including domain, such as username@DOMAIN or domain\username), and password. Some platforms will use the current Windows login. If you authenticate successfully, portal item results will display in the combo box below. Select a web map item and click the "load web map" button to display it in the map view.

## How it works

1. The `AuthenticationManager` object is configured with a challenge handler that will prompt for a Windows login (username and password) if a secure resource is encountered.
2. When a search for portal items is performed against an IWA-secured portal, the challenge handler creates an `UserCredential` object from the information entered by the user or what was automatically obtained by the current Windows login.
3. If the user authenticates, the search returns a list of web maps from `PortalItem` objects and the user can select one to display as an `Map`.

## Relevant API

* `AuthenticationManager`
* `Portal`
* `UserCredential`

## About the data

This sample searches for web map portal items on a secure portal. To successfully run the sample, you need:
 - Access to a portal secured with Integrated Windows Authentication that contains one or more web map items.
 - A login that grants you access to the portal.

## Tags

Authentication, Portal, Security, Windows
