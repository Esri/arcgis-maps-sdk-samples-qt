// [WriteFile Name=DeleteFeaturesFeatureService, Category=EditData]
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
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import Esri.Samples 1.0

DeleteFeaturesFeatureServiceSample {
    id: deleteFeaturesSample
    width: 800
    height: 600

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    onFeatureSelected: {
        // show the callout
        callout.x = deleteFeaturesSample.screenX;
        callout.y = deleteFeaturesSample.screenY;
        callout.visible = true;
    }

    onHideWindow: {
        // hide the callout
        callout.visible = false;
    }

    // map callout window
    Rectangle {
        id: callout
        width: row.width + (10 * scaleFactor) // add 10 for padding
        height: 40 * scaleFactor
        radius: 5
        border {
            color: "lightgrey"
            width: .5
        }
        visible: false

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
        }

        Row {
            id: row
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                margins: 5 * scaleFactor
            }
            spacing: 10

            Text {
                text: deleteFeaturesSample.featureType
                font.pixelSize: 18 * scaleFactor
            }

            Rectangle {
                radius: 100
                width: 22 * scaleFactor
                height: width
                color: "transparent"
                antialiasing: true
                border {
                    width: 2
                    color: "red"
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: -4 * scaleFactor

                    text: "-"
                    font {
                        bold: true
                        pixelSize: 22 * scaleFactor
                    }
                    color: "red"
                }

                // create a mouse area over the (-) text to delete the feature
                MouseArea {
                    anchors.fill: parent
                    // once the delete button is clicked, hide the window and delete the feature
                    onClicked: {
                        callout.visible = false;
                        // Call C++ invokable function to delete the selected feature
                        deleteFeaturesSample.deleteSelectedFeature();
                    }
                }
            }
        }
    }
}
