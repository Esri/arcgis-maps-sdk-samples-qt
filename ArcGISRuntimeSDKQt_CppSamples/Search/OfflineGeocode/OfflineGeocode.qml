// [WriteFile Name=OfflineGeocode, Category=Search]
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
import Esri.Samples 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Controls 100.3

OfflineGeocodeSample {
    id: offlineGeocodeSample
    clip: true

    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor
    property string dataPath: System.resolvedPath(System.userHomePath) + "/ArcGIS/Runtime/Data/"

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"

        Callout {
            id: callout
            calloutData: offlineGeocodeSample.calloutData
            screenOffsetY: -19 * scaleFactor
            accessoryButtonHidden: true
        }
    }

    // address bar and suggestions list
    Column {
        anchors {
            fill: parent
            margins: 10 * scaleFactor
        }

        Rectangle {
            id: addressSearchRect
            anchors {
                left: parent.left
                right: parent.right
            }

            height: 35 * scaleFactor
            color: "#f7f8fa"
            border {
                color: "#7B7C7D"
                width: 1 * scaleFactor
            }
            radius: 2

            Row {
                anchors.centerIn: parent
                width: parent.width
                height: parent.height
                leftPadding: 5 * scaleFactor

                // search bar for geocoding
                TextField {
                    id: textField
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width * 0.90
                    height: parent.height * 0.90
                    opacity: 0.95
                    placeholderText: "Enter an Address"

                    style: TextFieldStyle {
                        background: Rectangle {
                            color: "#f7f8fa"
                            radius: 2
                        }
                    }

                    // set the SuggestListModel searchText whenever text is changed
                    onTextChanged: {
                        offlineGeocodeSample.setSuggestionsText(text);
                    }

                    // when enter or return is pressed, geocode with the inputted text
                    onAccepted: {
                        suggestionRect.visible = false;
                        offlineGeocodeSample.geocodeWithText(text);
                    }

                    // initial text
                    Component.onCompleted: {
                        text = "910 N Harbor Dr, San Diego, CA 92101";
                        suggestionRect.visible = false;
                    }
                }

                // button to close and open suggestions
                Rectangle {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                        margins: 5 * scaleFactor
                    }

                    width: 35 * scaleFactor
                    color: "#f7f8fa"
                    radius: 2

                    Image {
                        anchors.centerIn: parent
                        width: parent.width
                        height: parent.width
                        source: suggestionRect.visible ? "qrc:/Samples/Search/OfflineGeocode/ic_menu_closeclear_light_d.png" : "qrc:/Samples/Search/OfflineGeocode/ic_menu_collapsedencircled_light_d.png"

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                suggestionRect.visible = !suggestionRect.visible;
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: suggestionRect
            width: addressSearchRect.width
            height: 20 * suggestView.count * scaleFactor
            color: "#f7f8fa"
            opacity: 0.85
            visible: false

            ListView {
                id: suggestView
                model: offlineGeocodeSample.suggestions
                height: parent.height
                delegate: Component {

                    Rectangle {
                        width: addressSearchRect.width
                        height: 20 * scaleFactor
                        color: "#f7f8fa"
                        border.color: "darkgray"

                        Text {
                            anchors {
                                verticalCenter: parent.verticalCenter
                                margins: 10 * scaleFactor
                            }

                            font {
                                weight: Font.Black
                                pixelSize: 12 * scaleFactor
                            }

                            text: label
                            elide: Text.ElideRight
                            leftPadding: 5 * scaleFactor
                            renderType: Text.NativeRendering
                            color: "black"
                        }

                        // when user clicks suggestion, geocode with the selected address
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                offlineGeocodeSample.geocodeWithSuggestion(index);
                                textField.text = label;
                                suggestionRect.visible = false;
                            }
                        }
                    }
                }
            }
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        visible: offlineGeocodeSample.geocodeInProgress
    }

    Rectangle {
        id: noResultsRect
        anchors.centerIn: parent
        height: 50 * scaleFactor
        width: 200 * scaleFactor
        color: "#f7f8fa"
        visible: offlineGeocodeSample.noResults
        radius: 2
        opacity: 0.85
        border.color: "black"

        Text {
            anchors.centerIn: parent
            text: "No matching address"
            renderType: Text.NativeRendering
            font.pixelSize: 18 * scaleFactor
        }
    }

    // signal handler to dismiss suggestions when user clicks away
    onDismissSuggestions: {
        suggestionRect.visible = false;
    }

    // if suggestions changed, make them visible
    onSuggestInProgressChanged: {
        if(offlineGeocodeSample.suggestInProgress)
            suggestionRect.visible = true;
    }

    MessageDialog {
        visible: text.length > 0
        text: errorMessage
        informativeText: "please consult README.md"
    }
}
