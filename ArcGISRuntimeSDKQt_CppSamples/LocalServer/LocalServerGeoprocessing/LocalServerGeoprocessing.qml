// [WriteFile Name=LocalServerFeatureLayer, Category=LocalServer]
// [Legal]
// Copyright 2017 Esri.

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
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import Esri.Samples 1.0

LocalServerGeoprocessingSample {
    id: localServerGeoprocessingSample
    width: 800
    height: 600

    property double scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    // Create MapQuickView here, and create its Map etc. in C++ code
    MapView {
        anchors.fill: parent
        objectName: "mapView"
        // set focus to enable keyboard navigation
        focus: true

        Rectangle {
            id: actionAreaRect
            anchors {
                margins: 15 * scaleFactor
                top: parent.top
                left: parent.left
            }
            width: parent.width * 0.3 * scaleFactor
            height: parent.height * 0.30 * scaleFactor
            color: isReady ? "black" : "silver"
            opacity: 0.7
            radius: 5 * scaleFactor

            Column {
                id: contentColumn
                spacing: 5 * scaleFactor
                anchors.fill: parent
                anchors.margins: 5 * scaleFactor
                visible: isReady

                Text {
                    text: qsTr("Specify the interval, or distance, between contour lines and click the 'Generate Contours' button.")
                    width: parent.width
                    wrapMode: Text.WordWrap
                    color: "white"
                    font.pixelSize: 12 * scaleFactor
                }

                Rectangle {
                    width: parent.width
                    height: 35 * scaleFactor
                    radius: 5 * scaleFactor

                    Text {
                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                            margins: 5 * scaleFactor
                        }
                        width: 40 * scaleFactor
                        text: qsTr("Interval: ")
                        font.bold: true
                        font.pixelSize: 14 * scaleFactor
                    }

                    TextField {
                        id: intervalText
                        anchors {
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                            margins: 5 * scaleFactor
                        }
                        text: "200"
                        validator: DoubleValidator {bottom: 100; top: 500}
                    }
                }

                Button {
                    width: parent.width
                    text: "Generate Contours"
                    height: 35 * scaleFactor
                    font.pixelSize: 14
                    font.bold: true
                    onClicked: {
                        generateContours(intervalText.text);
                    }
                }

                Button {
                    width: parent.width
                    text: "Clear Results"
                    height: 35 * scaleFactor
                    font.pixelSize: 14
                    font.bold: true
                    onClicked: {
                        clearResults();
                    }
                }

            }

            BusyIndicator {
                id: busyIndicator
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                width: 22 * scaleFactor
                height: width
                running: !isReady
            }
        }
    }
}


