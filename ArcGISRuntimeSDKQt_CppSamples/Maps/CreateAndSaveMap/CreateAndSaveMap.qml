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
import QtQuick.Controls 2.2
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.9

CreateAndSaveMapSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600    

    onPortalLoaded: {
        stackView.push(options);
    }

    onSaveMapCompleted: {
        if (stackView.currentItem === completionRect)
            return;

        const url =  "https://www.arcgis.com/home/item.html?id=%1".arg(itemId);
        stackView.push(completionRect,
                       { text: success ? 'Map saved successfully.<br>View in <a href="%1">ArcGIS Online</a>'.arg(url)
                                       : "An error occurred while saving the map. Details: %1".arg(error)
                       });
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: layerWindow
    }

    // add a mapView component
    MapView {
        id: mapView
        objectName: "mapView"
        Button {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: mapView.attributionTop
                margins: 5
            }
            text: "Save map"

            onClicked: {
                loadPortal();
            }
        }
    }

    Component {
        id: layerWindow
        LayerWindow {
            onCreateMapSelected: {
                stackView.push(mapView);
                createMap(basemap, layerList);
            }
        }
    }

    // Window to display options for setting title, tags, and description
    Component {
        id: options
        SaveOptionsWindow {
            onCancelClicked: {
                stackView.pop();
            }

            onSaveMapClicked: {
                saveMap(title, tags, description);
            }
        }
    }

    // Rectangle to display completion text
    Component {
        id: completionRect
        Rectangle {
            property alias text: completeText.text

            Text {
                id: completeText
                anchors.centerIn: parent

                textFormat: Text.RichText
                horizontalAlignment: Text.AlignHCenter
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Button {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    margins: 5
                }
                text: "Create New Map"
                onClicked: {
                    // We need a local ref to the stackView and layerWindow
                    // object as our object references will have been deleted
                    // once "clear" cleans up this object.
                    const sv = stackView;
                    const lWindow = layerWindow;
                    sv.clear();
                    sv.push(lWindow);
                }
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
