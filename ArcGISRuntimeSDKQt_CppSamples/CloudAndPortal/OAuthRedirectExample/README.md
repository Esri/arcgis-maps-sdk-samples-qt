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

## Setting up an App with a custom Url protocol
The custom Url protocol defines a base url which will be handled by the App itself rather than a browser.

### Register your App online
The 1st step is to register a new app by visiting: 

https://developers.arcgis.com/applications/#/c4aafa91face4b7db12400e1d18a04fd/auth/

Once you have logged on you can "Register New Application" to set up your app.

Make a note of the `Client Id` for the app - this is what you will use in your app to begin the log-in process.

### Add a Redirect URI
Switch to the "Authentication" tab and scroll down to "Redirect URIs" section at the bottom of the page.

Add the custom url scheme you wish to use with the app - e.g. "myapp". This will be the base for the 
redirect Url that your app will be set to handle.

### Add the ClientID to your app
Open the file `OAuthRedirectExample.pro` file and set your ClientID which you obtained when you registered
you app online to be the new value for `CLIENT_ID` - e.g. DEFINES +="CLIENT_ID=\"XXXXXXXXXXX\"" where "XXXXXXXXXXX" is your ClientID.


## Register the custom URL scheme
Registering a custom URL scehem is handled differently on each platform. On some platforms there are a number of ways this can be achieved. This sample outlines some approaches for each of the supported platforms which can be used while developing. When releasing a product you will need to determine the best way to carry out the registration of the url scheme on your user's systems.

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

### Linux
_NOTE - this approach for defining a Url scheme on Linux will only work when Mozilla Firefox is the browser used for authentication._

First build the project and take a note of the absolute path to the app - check the app can be run directly by entering this path from a terminal. There are a number of ways to record the protocol for Firefox:

#### Option 1:
In a terminal, type:

    `gconftool-2 -s /desktop/gnome/url-handlers/foo/command '/path/to/app %s' --type String`
    `gconftool-2 -s /desktop/gnome/url-handlers/foo/enabled --type Boolean true`

Replace foo on both lines with the protocol you want to register and /path/to/app with the path to your app.

#### Option 2:
Alternatively, in the Firefox browser, type `about:config` into the address bar and hit Enter.
Right-click -> New -> Boolean -> Name: network.protocol-handler.expose.foo -> Value -> false (Replace foo with the protocol you're specifying)

Next time you click a link of protocol-type foo you will be asked which application to open it with - enter the path to your app. You can try out the protocol by entering `myapp://hello` in a new Firefox tab. 


When the app is launched from the browser a new instance will be launched: however, in this example,
the desired behaviour is that the same instance of the app should be notified with the authorization code. To achieve this, on linux the sample makes use of the `QtSingleApplication` from qt-solutions. Using this type, the original instance is notified of the authorization code as a command line argument.

### Mac
In order to register a custom URL protocol on Mac whilst developing, you can define a scheme in XCode.

First, build the project in QtCreator. Next, Tt create an XCode project from QtCreator add the follwoing additional arguments add additional arguments to the project's qmake build settings

    `-spec macx-xcode`

Open the generated project (e.g. `OAuthRedirectExample.xcodeproj`) in XCode. 

You may need to adjust the Qt Postlink step created by QtCreator to point to the target which XCode will build (e.g. to reference the OAuthRedirectExample.app in the sub directory for Debug or Release). E.g. Go to Build Phases/Qt PostLink and edit the script to look something like:

`
install_name_tool -change libEsriCommonQt.dylib "~/ArcGIS_SDKs/Qt100.1/sdk/macOS/x64/lib/libEsriCommonQt.dylib" Release/OAuthRedirectExample.app/Contents/MacOS/OAuthRedirectExample
`

Next go to Info/URL Types and add a new URL type with Identifier and URL Schemes set to the custom URL scheme you set up for your app.

Now when you build and run the Project from XCode, the url scheme will be recognised and the URL will be treated as a `QFileOpenEvent`.

## QtSingleApplication dependancy
On Windows and Linux, the sample depends upon the `QtSingleApplication` type from qt-solutions. You can download the project from 
https://github.com/qtproject/qt-solutions. The sample is designed to include the source files ditrectly: update the `QT_SINGLEAPP_DIR` 
variable in `OAuthRedirectExample.pro` to point to the location you unpack it to.

## Features
- AuthenticationChallenge
- AuthenticationManager
- Portal
