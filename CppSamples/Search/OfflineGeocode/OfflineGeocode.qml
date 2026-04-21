// [WriteFile Name=OfflineGeocode, Category=Search]
// [Legal]
// Copyright 2016 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

import QtQuick
import Esri.Samples
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Layouts
import Esri.ArcGISRuntime.Toolkit

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

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Callout {
            id: callout
            leaderPosition: Callout.LeaderPosition.Automatic
            calloutData: mapView.calloutData
            screenOffsetY: -19
            accessoryButtonVisible: false
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

            color: palette.base
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

                    placeholderText: qsTr("Enter an Address")

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
                        text = qsTr("910 N Harbor Dr, San Diego, CA 92101")
                        suggestionRect.visible = false;
                    }
                }

                // button to close and open suggestions
                ToolButton {
                    Layout.rightMargin: 5
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                    width: height
                    height: textField.contentHeight
                    padding: 0
                    display: AbstractButton.IconOnly
                    icon {
                        source: suggestionRect.visible ? "qrc:/Samples/Search/OfflineGeocode/x-24.svg" : "qrc:/Samples/Search/OfflineGeocode/chevron-down-24.svg"
                        width: textField.contentHeight
                        height: textField.contentHeight
                        color: palette.text
                    }
                    onClicked: {
                        suggestionRect.visible = !suggestionRect.visible
                    }

                    background: Rectangle {
                        color: "transparent"
                        radius: 2
                    }
                }
            }
        }

        Rectangle {
            id: suggestionRect
            width: addressSearchRect.width
            height: 20 * suggestView.count
            color: palette.base
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
                        color: palette.mid
                        border.color: "darkgray"

                        Label {
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
        color: palette.base
        visible: offlineGeocodeSample.noResults
        radius: 2
        opacity: 0.85
        border.color: "black"

        Label {
            anchors.centerIn: parent
            text: qsTr("No matching address")
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
        title: qsTr("Error")
        property alias text : textLabel.text
        property alias informativeText : detailsLabel.text
        ColumnLayout {
            Label {
                id: textLabel
                text: errorMessage
            }
            Label {
                id: detailsLabel
                text: qsTr("please consult README.md")
            }
        }
    }
}
