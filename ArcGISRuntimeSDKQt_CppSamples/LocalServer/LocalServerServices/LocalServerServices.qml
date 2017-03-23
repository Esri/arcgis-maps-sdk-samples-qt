// [WriteFile Name=LocalServerServices, Category=LocalServer]
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
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
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
                id: topRow
                spacing: 10 * scaleFactor

                Button {
                    id: startButton
                    text: "Start Local Server"
                    width: localServerServicesSample.width * 0.475

                    onClicked: {
                        startLocalServer();
                    }
                }

                Button {
                    id: stopButton
                    text: "Stop Local Server"
                    width: localServerServicesSample.width * 0.475
                    enabled: isServerRunning & !isServiceRunning

                    onClicked: {
                        stopLocalServer();
                    }
                }

                            }

            Row {
                spacing: 10 * scaleFactor

                ComboBox {
                    id: servicesCombo
                    width: startServiceButton.width
                    enabled: isServerRunning
                    model: ["Map Service", "Feature Service", "Geoprocessing Service"]

                    onCurrentIndexChanged: {
                        filePathText.text = "";
                    }
                }

                TextField {
                    id: filePathText
                    placeholderText: "Browse for a file."
                    width: startServiceButton.width - (40 * scaleFactor)
                }

                Button {
                    id: fileDialogButton
                    text: "..."
                    width: 30 * scaleFactor
                    enabled: isServerRunning

                    onClicked: {
                        fileDialog.visible = true;
                    }
                }
            }

            Row {
                spacing: 10 * scaleFactor

                Button {
                    id: startServiceButton
                    text: "Start Service"
                    width: localServerServicesSample.width * 0.475
                    enabled: isServerRunning

                    onClicked: {
                        startService(servicesCombo.currentText, filePathText.text);
                    }
                }

                Button {
                    id: stopServiceButton
                    text: "Stop Service"
                    width: localServerServicesSample.width * 0.475
                    enabled: isServiceRunning

                    onClicked: {
                        stopService(servicesView.currentValue);
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
                model: servicesList.length
                delegate: servicesDelegate
                property string currentValue: ""
            }

            Button {
                anchors {
                    right: parent.right
                }
                text: "Open Url"
                visible: servicesView.model > 0

                onClicked: {
                    openURL(servicesView.currentValue);
                }
            }
        }
    }

    Component {
        id: servicesDelegate
        Rectangle {
            id: rect
            width: parent.width
            height: 35 * scaleFactor
            color: (index % 2) ? "white" : "#DDF3EF"
            Text {
                text: servicesList[index]
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    leftMargin: 5 * scaleFactor
                }
                elide: Text.ElideLeft
                font.pixelSize: 14 * scaleFactor

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        rect.color = "lightgrey"
                        servicesView.currentValue = servicesList[index];
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: System.userHomeFolder.url + "/ArcGIS/Runtime/Data"
        nameFilters: servicesCombo.currentIndex === 0 || servicesCombo.currentIndex === 1 ? ["mpks (*.mpk)", "All files (*)"] : ["gpks (*.gpk)", "All files (*)"]
        onAccepted: {
            filePathText.text = fileUrl;
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


