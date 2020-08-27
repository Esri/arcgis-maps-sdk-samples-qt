// [WriteFile Name=TokenAuthentication, Category=CloudAndPortal]
// [Legal]
// Copyright 2016 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

import QtQuick 2.6
import QtQuick.Controls 2.2
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.9

TokenAuthenticationSample {
    id: authSample
    clip: true
    width: 800
    height: 600
    
    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        Column {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: mapView.attributionTop
                margins: 10
            }
            spacing: 10

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 14
                text: "username/password: user1/user1"
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Load Secured Layer"
                onClicked: loadSecuredLayer();
            }
        }
    }

    // Uncomment this section when running as standalone application
    /*
    // Declare an AuthenticationView
    AuthenticationView {
        anchors.fill: parent
        authenticationManager: authSample.authenticationManager // set the authenticationManager property (this needs to be registered)
    }
    */
}
