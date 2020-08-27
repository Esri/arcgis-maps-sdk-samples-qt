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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Esri.ArcGISRuntime.Toolkit.Controls 100.9

OfflineGeocodeSample {
    id: offlineGeocodeSample
    clip: true
    width: 800
    height: 600    

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        Callout {
            id: callout
            leaderPosition: leaderPositionEnum.Automatic
            calloutData: mapView.calloutData
            screenOffsetY: -19
            accessoryButtonHidden: true
        }
    }

    // address bar and suggestions list
    Column {
        anchors {
            fill: parent
            margins: 10
        }

        Rectangle {
            id: addressSearchRect
            anchors {
                left: parent.left
                right: parent.right
            }
            height: childrenRect.height

            color: "#f7f8fa"
            border {
                color: "#7B7C7D"
                width: 1
            }
            radius: 2

            RowLayout {
                width: parent.width
                height: childrenRect.height

                // search bar for geocoding
                TextField {
                    id: textField
                    Layout.fillWidth: true
                    leftPadding: 5
                    selectByMouse: true

                    placeholderText: "Enter an Address"

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
                    Layout.margins: 5
                    width: height
                    height: textField.contentHeight
                    color: "#f7f8fa"
                    radius: 2

                    Image {
                        anchors.fill: parent
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
            height: 20 * suggestView.count
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
                        height: 20
                        color: "#f7f8fa"
                        border.color: "darkgray"

                        Text {
                            anchors {
                                verticalCenter: parent.verticalCenter
                                margins: 10
                            }

                            font {
                                weight: Font.Black
                                pixelSize: 12
                            }

                            text: label
                            elide: Text.ElideRight
                            leftPadding: 5
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
        height: 50
        width: 200
        color: "#f7f8fa"
        visible: offlineGeocodeSample.noResults
        radius: 2
        opacity: 0.85
        border.color: "black"

        Text {
            anchors.centerIn: parent
            text: "No matching address"
            font.pixelSize: 18
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

    Dialog {
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        visible: text.length > 0
        title: "Error"
        property alias text : textLabel.text
        property alias informativeText : detailsLabel.text
        ColumnLayout {
            Text {
                id: textLabel
                text: errorMessage
            }
            Text {
                id: detailsLabel
                text: "please consult README.md"
            }
        }
    }
}
