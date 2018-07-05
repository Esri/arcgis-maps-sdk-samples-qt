// [WriteFile Name=FindAddress, Category=Search]
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
import QtQuick.Controls.Styles 1.4
import Esri.ArcGISRuntime 100.3
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property string calloutText
    property string calloutDetailText
    property double mousePointX
    property double mousePointY

    // Create MapView that contains a Map with the Imagery with Labels Basemap
    MapView {
        id: mapView
        anchors.fill: parent

        // Create a GraphicsOverlay to display the geocode results
        GraphicsOverlay {
            id: graphicsOverlay

            SimpleRenderer {
                PictureMarkerSymbol {
                    url: "qrc:/Samples/Search/FindAddress/pin_circle_red.png"
                    width: 35
                    height: 35
                    offsetY: height / 2 // the tip of the pin will point to the location
                }
            }
        }

        Map {
            BasemapImageryWithLabels {}
        }

        // show the callout once the identify is complete
        onIdentifyGraphicsOverlayStatusChanged: {
            if (identifyGraphicsOverlayStatus === Enums.TaskStatusCompleted) {
                if (identifyGraphicsOverlayResult.graphics.length > 0) {
                    // setup the attributes
                    calloutText = identifyGraphicsOverlayResult.graphics[0].attributes.attributeValue("Match_addr");
                    calloutDetailText = identifyGraphicsOverlayResult.graphics[0].attributes.attributeValue("Place_addr");

                    // show the callout
                    callout.x = mousePointX - (callout.width / 2);
                    callout.y = mousePointY - (60 * scaleFactor);
                    callout.visible = true;
                }
            }
        }

        // perform identify operation on mapview
        onMouseClicked: {
            callout.visible = false;
            mousePointX = mouse.x;
            mousePointY = mouse.y;
            mapView.identifyGraphicsOverlay(graphicsOverlay, mouse.x, mouse.y, 2, false);
        }

        // hide callout after navigation
        onViewpointChanged: {
            callout.visible = false;
        }
    }

    //! [FindAddress create LocatorTask]
    // Create a locator task using the World Geocoding Service
    LocatorTask {
        id: locatorTask
        url: "http://geocode.arcgis.com/arcgis/rest/services/World/GeocodeServer"

        // handle the result once the geocode status is complete
        onGeocodeStatusChanged: {
            if (geocodeStatus === Enums.TaskStatusCompleted) {
                if (geocodeResults.length > 0) {
                    graphicsOverlay.graphics.clear();
                    var graphic = ArcGISRuntimeEnvironment.createObject("Graphic");
                    graphic.geometry = geocodeResults[0].displayLocation;
                    graphic.attributes.attributesJson = geocodeResults[0].attributes;
                    graphicsOverlay.graphics.append(graphic);
                    mapView.setViewpointCenterAndScale(geocodeResults[0].extent.center, 8000);
                }
            }
        }
    }

    // Create geocode parameters
    GeocodeParameters {
        id: geocodeParameters
        minScore: 75
        resultAttributeNames: ["Place_addr", "Match_addr"]
    }
    //! [FindAddress create LocatorTask]

    // map callout window
    Rectangle {
        id: callout
        width: 225 * scaleFactor
        height: 40 * scaleFactor
        radius: 5
        border {
            color: "lightgrey"
            width: .5 * scaleFactor
        }
        clip: true
        visible: false

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
        }

        Column {
            anchors {
                fill: parent
                margins: 5 * scaleFactor
            }
            spacing: 2

            Text {
                width: parent.width
                text: calloutText
                font.pixelSize: 14 * scaleFactor
                elide: Text.ElideRight
            }
            Text {
                width: parent.width
                text: calloutDetailText
                font.pixelSize: 10 * scaleFactor
                elide: Text.ElideRight
            }
        }
    }

    // search bar for geocoding
    Column {
        anchors {
            fill: parent
            margins: 10 * scaleFactor
        }

        Row {
            width: parent.width
            height: 35 * scaleFactor
            spacing: 5

            TextField {
                id: textField
                width: parent.width
                height: parent.height
                placeholderText: "Type in an address"
                font.pixelSize: 14 * scaleFactor
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

                Keys.onEnterPressed: geocodeAddress();
                Keys.onReturnPressed: geocodeAddress();

                function geocodeAddress() {
                    //! [FindAddress geocodeWithParameters]
                    locatorTask.geocodeWithParameters(textField.text, geocodeParameters);
                    //! [FindAddress geocodeWithParameters]
                    suggestView.visible = false;
                    Qt.inputMethod.hide();
                }

                Rectangle {
                    anchors {
                        right: parent.right
                        top: parent.top
                        bottom: parent.bottom
                        margins: 5 * scaleFactor
                    }

                    width: 35 * scaleFactor
                    color: "#f7f8fa"

                    Image {
                        anchors.centerIn: parent
                        width: 35 * scaleFactor
                        height: width
                        source: "qrc:/Samples/Search/FindAddress/ic_menu_collapsedencircled_light_d.png"
                        visible: textField.text.length === 0

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                textField.focus = true;
                                suggestView.visible = !suggestView.visible;
                            }
                        }
                    }
                }

                Image {
                    anchors {
                        right: parent.right
                        top: parent.top
                        bottom: parent.bottom
                        margins: 5 * scaleFactor
                    }
                    source: "qrc:/Samples/Search/FindAddress/ic_menu_closeclear_light_d.png"
                    width: 27 * scaleFactor
                    height: width
                    visible: parent.text.length !== 0

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            textField.text = "";
                            callout.visible = false;
                            graphicsOverlay.graphics.clear();
                        }
                    }
                }
            }
        }

        // show a drop down of suggested locations
        ListView {
            id: suggestView
            height: 300 * scaleFactor
            width: textField.width
            visible: false
            clip: true
            model: geocodeSuggestions
            delegate: Component {
                Rectangle {
                    id: rect
                    width: parent.width
                    height: 25 * scaleFactor
                    color: "#f7f8fa"

                    Rectangle {
                        anchors {
                            top: parent.top;
                            left: parent.left;
                            right: parent.right;
                            topMargin: -5 * scaleFactor
                            leftMargin: 20 * scaleFactor
                            rightMargin: 20 * scaleFactor
                        }
                        color: "darkgrey"
                        height: 1
                    }

                    Text {
                        text: name
                        anchors {
                            fill: parent
                            leftMargin: 5 * scaleFactor
                        }

                        font.pixelSize: 14 * scaleFactor
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            textField.text = name;
                            suggestView.visible = false;
                            locatorTask.geocodeWithParameters(name, geocodeParameters);
                            Qt.inputMethod.hide();
                        }
                    }
                }
            }
        }
    }

    ListModel {
        id: geocodeSuggestions
        ListElement { name: "277 N Avenida Caballeros, Palm Springs, CA" }
        ListElement { name: "380 New York St, Redlands, CA 92373" }
        ListElement { name: "Београд" }
        ListElement { name: "Москва" }
        ListElement { name: "北京" }
    }
}
