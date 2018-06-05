// [WriteFile Name=CreateAndSaveMap, Category=Maps]
// [Legal]
// Copyright 2018 Esri.

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
import QtQuick.Controls 1.4
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.3

CreateAndSaveMapSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    onPortalLoaded: {
        options.visible = true;
        options.reset();
        stackView.push(options);
    }

    onSaveMapCompleted: {
        if (stackView.currentItem === completionRect)
            return;

        if (success) {
            completeText.webmapUrl = "https://www.arcgis.com/home/item.html?id=%1".arg(itemId);
            completeText.text = 'Map saved successfully.<br>View in <a href="%1">ArcGIS Online</a>'.arg(completeText.webmapUrl);
        } else {
            completeText.text = "An error occurred while saving the map. Details: %1".arg(error);
        }
        stackView.push(completionRect);
    }

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: LayerWindow {
            id: layerWindow
            onCreateMapSelected: {
                mapView.visible = true;
                stackView.push(mapView)
                createMap(basemap, layerList);
            }
        }
    }

    // add a mapView component
    MapView {
        id: mapView
        objectName: "mapView"
        visible: false

        Button {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: mapView.attributionTop
                margins: 5 * scaleFactor
            }
            text: "Save map"

            onClicked: {
                loadPortal();
            }
        }
    }

    // Window to display options for setting title, tags, and description
    SaveOptionsWindow {
        id: options
        visible: false

        onCancelClicked: {
            mapView.visible = true;
            stackView.pop(mapView)
        }

        onSaveMapClicked: {
            saveMap(title, tags, description);
        }
    }

    // Rectangle to display completion text
    Rectangle {
        id: completionRect

        Text {
            id: completeText
            anchors.centerIn: parent
            width: 200 * scaleFactor
            wrapMode: Text.Wrap

            property string webmapUrl

            textFormat: Text.RichText
            horizontalAlignment: Text.AlignHCenter
            onLinkActivated: Qt.openUrlExternally(webmapUrl)
        }

        Button {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                margins: 10 * scaleFactor
            }
            text: "Create New Map"
            onClicked: {
                stackView.clear();
                stackView.push(layerWindow)
            }
        }
    }

    /*
    // Declare AuthenticationView to handle any authentication challenges
    AuthenticationView {
        anchors.fill: parent
        authenticationManager: rootRectangle.authenticationManager
    }
    */
}
