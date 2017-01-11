#PortalUser Info

This sample demonstrates how to retrieve a user's details via a Portal.

![](screenshot.png)

##How it works
A `Portal` is created, and supplied a `Credential` which uses OAuth in user mode. 
When the app launches, the portal is loaded, which triggers an authentication challenge.

An `AuthenticationView` listens to the challenge and displays a login screen to allow user credentials to be entered.

If the portal is successfully loaded, the `portalUser` property is used to populate a series of fields including:
 - `fullName`
 - `username`
 - `email`
 - `description`
 - `access`

 The `thumbnailUrl` property is bound to a QML Image to display the user's avatar.

Similarly, the `portalInfo` property is used to populate:
 - `organizationName`
 - `organizationDescription`
 - `thumbnailUrl`
 - `canSearchPublic`
 - `canSharePublic`
 
##Features
- Portal
- Credential
- PortalUser
- PortalInfo
- AuthenticationView
- AuthenticationManager
