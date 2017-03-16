// [WriteFile Name=ChangeBasemap, Category=Maps]
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
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

LocalServerServices {
    id: localServerServicesSample
    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor

    Rectangle {
        id: mainRect
        anchors.fill: parent
        anchors.margins: 15 * scaleFactor

        Column {
            spacing: 10 * scaleFactor
            anchors.fill: parent

            Row {
                spacing: 10 * scaleFactor

                Button {
                    id: startButton
                    text: "Start Local Server"
                    width: localServerServicesSample.width * 0.25

                    onClicked: {
                        startLocalServer();
                    }
                }

                ComboBox {
                    id: servicesCombo
                    width: (startServiceButton.width * 2) + (10 * scaleFactor)

                    model: ["Map Service", "Feature Service", "Geoprocessing Service"]
                    enabled: isServerRunning
                }

            }

            Row {
                spacing: 10 * scaleFactor

                Button {
                    id: stopButton
                    text: "Stop Local Server"
                    width: localServerServicesSample.width * 0.25
                    enabled: isServerRunning

                    onClicked: {
                        stopLocalServer();
                    }
                }

                Button {
                    id: startServiceButton
                    text: "Start Service"
                    width: localServerServicesSample.width * 0.343
                    enabled: isServerRunning

                    onClicked: {
                        startService(servicesCombo.currentText);
                    }
                }

                Button {
                    id: stopServiceButton
                    text: "Stop Service"
                    width: localServerServicesSample.width * 0.343
                    enabled: isServiceRunning

                    onClicked: {
                        stopService(servicesCombo.currentText);
                    }
                }
            }

            TextArea {
                id: serverStatusTextArea
                width: startButton.width + servicesCombo.width + (10 * scaleFactor)
                height: 200 * scaleFactor
                text: serverStatus
            }

            Text {
                text: "List of Running Services"
            }

            ListView {
                id: servicesView
                width: startButton.width + servicesCombo.width + (10 * scaleFactor)
                height: 200 * scaleFactor
                model: services
                delegate: servicesDelegate
                property string currentValue: ""
            }


        }

        Button {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            text: "Go to URL"

            onClicked: {
                openURL(servicesView.currentValue);
            }
        }
    }

    Component {
        id: servicesDelegate
        Rectangle {
            id: rect
            width: parent.width
            height: 35 * scaleFactor

            Text {
                text: display
                anchors {
                    fill: parent
                    leftMargin: 5 * scaleFactor
                }
                elide: Text.ElideLeft
                font.pixelSize: 14 * scaleFactor

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        servicesView.currentIndex = index;
                        servicesView.currentValue = display;
                        rect.color = "lightgray";
                    }
                }
            }

            Rectangle {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    topMargin: -8 * scaleFactor
                }
                color: "darkgrey"
                height: 1 * scaleFactor
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}


