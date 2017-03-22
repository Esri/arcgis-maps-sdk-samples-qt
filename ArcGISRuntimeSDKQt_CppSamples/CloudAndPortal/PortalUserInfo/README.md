# PortalUser Info

This sample demonstrates how to retrieve a user's details via a Portal

![](screenshot.png)

## How it works
A `Portal` is created, and supplied a `Credential` which uses OAuth in user mode. 
When the app launches, the portal is loaded, whcih triggers an authentication challenge.

An `AuthenticationView` listens to the challenge and displays a login screen to allow user credentials to be entered.

If the portal is successfully loaded, the `portalUser` property is used to populate a series of fields including:
 - `fullName`
 - `username`
 - `email`
 - `description`
 - `access`

 The `thumbnailUrl` property is bound to a QML Image to display the user's avatar.
 
## Features
- Portal
- Credential
- PortalUser
- AuthenticationView
- AuthenticationManager
