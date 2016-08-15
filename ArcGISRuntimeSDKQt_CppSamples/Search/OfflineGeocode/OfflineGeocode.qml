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
import Esri.ArcGISExtras 1.1
import QtQuick.Controls.Styles 1.4
import Esri.ArcGISRuntime.Toolkit.Controls 2.0

OfflineGeocodeSample {
    id: offlineGeocodeSample
    clip: true

    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor
    property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data"
    property real suggestionHeight: 20

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"

        Callout {
            id: callout
            calloutData: offlineGeocodeSample.calloutData
            screenOffsety: -29 * scaleFactor
        }
    }

    Column {
        anchors {
            fill: parent
            margins: 10 * scaleFactor
        }

        Rectangle {
            width: 300 * scaleFactor
            height: 35 * scaleFactor
            color: "#f7f8fa"

            Row {
                width: parent.width
                height: parent.height

                TextField {
                    id: textField
                    width: parent.width
                    height: parent.height
                    opacity: 0.95
                    placeholderText: "Enter an Address"

                    style: TextFieldStyle {
                        background: Rectangle {
                            color: "#f7f8fa"
                            border {
                                color: "#7B7C7D"
                                width: 1 * scaleFactor
                            }
                            radius: 2
                        }
                    }

                    // when user types, suggestions appear
                    Keys.onPressed: {
                        suggestionRect.visible = true;
                        offlineGeocodeSample.setSuggestionsText(text);
                    }

                    onAccepted: {
                        suggestionRect.visible = false;
                        offlineGeocodeSample.geocodeWithText(text);
                    }
                }

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
                                suggestionRect.visible ? suggestionRect.visible = false : suggestionRect.visible = true
                            }
                        }
                    }
                }

                BusyIndicator {
                    id: suggestBusyIndicator
                    anchors.verticalCenter: parent.verticalCenter
                    height: 25 * scaleFactor
                    running: offlineGeocodeSample.suggestInProgress && suggestionRect.visible
                }
            }
        }

        Rectangle {
            id: suggestionRect
            width: textField.width
            height: suggestionHeight * suggestView.count * scaleFactor
            color: "#f7f8fa"
            opacity: 0.85
            visible: false

            ListView {
                id: suggestView
                model: offlineGeocodeSample.suggestions
                height: parent.height
                delegate: Component {

                    Rectangle {
                        width: textField.width
                        height: suggestionHeight * scaleFactor
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

                            text: modelData
                            elide: Text.ElideRight
                            leftPadding: 5 * scaleFactor
                            renderType: Text.NativeRendering
                            color: "black"
                        }

                        // when user clicks suggestion, geocode with the selected address
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                suggestView.currentIndex = index;
                                suggestionRect.visible = false;
                                textField.text = label;
                                offlineGeocodeSample.geocodeWithSuggestion(index);
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
        color: "lightgrey"
        visible: offlineGeocodeSample.noResults
        radius: 2
        opacity: 0.85
        border.color: "black"

        Text {
            anchors.centerIn: parent
            text: "No matching addresses"
            renderType: Text.NativeRendering
            font.pixelSize: 18 * scaleFactor
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
