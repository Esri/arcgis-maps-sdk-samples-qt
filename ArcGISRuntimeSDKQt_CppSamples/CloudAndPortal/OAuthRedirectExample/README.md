# OAuth Redirect Example

This sample demonstrates an approach to using OAuth2 for authenticating with an ArcGIS portal.
Unlike the pattern used in other samples where an `AuthenticationView` displays a log-in screen 
using a webview within the app itself, this example sends an authorization request to an external
browser along with a redirect Uri. The user can then authenticate securely in the browser 
(using their ArcGIS.com credentials) and upon successful authentication, the redirect Uri 
is notified with an authorization code to allow the log-in process to continue.

In order for the call-back to the redirect Uri to be handled by the app, it is necessary to 
register a custom Url protocol on the user's operating system: the process for doing this is
different on each platform. 

## Setting up a custom Url protocol
The custom Url protocol defines a base url which will be handled by the App itself rather than a browser.

### Registering a redirect Uri
The first setp is to register the Url scheme with your arcgis.com app.

### Windows
On windows, the custom url protocol is setup using a registry setting which provides the path to an executable
which should be run when the url scheme is opened. This sample uses `QSettings` to set up the registry setting at runtime,
using the path to the apps executable.

In order for the exectuable to be runnable from the registry path, all dependencies for the app must be deployed even when
running from QtCreator. An example deployment script (`windows_deployment.bat`) has been included in the sample to 
help with this process but you will need to update the various variables used such as the location of the `QT_KIT`
and the `ARCGIS_SDK_DIR`.

When the executable is called via the registry path a new instance of the app will be launched: however, in this example,
the desired behaviour is that the same instance of the app should be notified with the authorization code. To achieve this,
on windows the sample makes use of the `QtSingleApplication` from qt-solutions. Using this type, the original instance 
is notified of the authorization code as a command line argument.

## QtSingleApplication dependancy
On Windows and Linux, the sample depends upon the `QtSingleApplication` type from qt-solutions. You can download the project from 
https://github.com/qtproject/qt-solutions. The sample is designed to include the source files ditrectly: update the `QT_SINGLEAPP_DIR` 
variable in `OAuthRedirectExample.pro` to point to the location you unpack it to.

## Features
- AuthenticationChallenge
- AuthenticationManager
- Portal
