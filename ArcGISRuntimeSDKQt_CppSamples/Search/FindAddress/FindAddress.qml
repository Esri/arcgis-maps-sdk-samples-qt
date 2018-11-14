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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit.Controls 100.5

FindAddressSample {
    id: findAddressSample
    width: 800
    height: 600

    

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"

        Callout {
            id: callout
            borderColor: "lightgrey"
            borderWidth: 1
            calloutData: findAddressSample.calloutData
            maxWidth: findAddressSample.width * 0.75
            leaderPosition: leaderPositionEnum.Automatic
            accessoryButtonHidden: true
        }
    }

    onShowCallout: {
        callout.showCallout();
    }

    onHideCallout: {
        if (callout.visible)
            callout.dismiss();
    }

    // search bar for geocoding
    Column {
        anchors {
            fill: parent
            margins: 10
        }
        Rectangle {
            color: "#f7f8fa"
            border {
                color: "#7B7C7D"
            }
            radius: 2
            width: parent.width
            height: childrenRect.height

            GridLayout {
                width: parent.width
                columns: 3
                TextField {
                    Layout.margins: 5
                    Layout.fillWidth: true
                    id: textField
                    font.pixelSize: 14
                    placeholderText: "Type in an address"

                    Keys.onEnterPressed: geocodeAddress();
                    Keys.onReturnPressed: geocodeAddress();

                    function geocodeAddress() {
                        findAddressSample.geocodeAddress(textField.text);
                        suggestView.visible = false;
                        Qt.inputMethod.hide();
                    }
                }

                Rectangle {
                    Layout.margins: 5
                    width: height
                    height: textField.height
                    color: "#f7f8fa"
                    visible: textField.length === 0
                    enabled: visible
                    Image {
                        anchors.fill: parent
                        source: "qrc:/Samples/Search/FindAddress/ic_menu_collapsedencircled_light_d.png"
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                textField.focus = true;
                                suggestView.visible = !suggestView.visible;
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.margins: 5
                    width: height
                    color: "transparent"
                    height: textField.height
                    visible: textField.length !== 0
                    enabled: visible
                    Image {
                        anchors.fill: parent
                        source: "qrc:/Samples/Search/FindAddress/ic_menu_closeclear_light_d.png"

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                textField.text = "";
                                if (callout.visible)
                                    callout.dismiss();
                                clearGraphics();
                            }
                        }
                    }
                }
            }
        }

        // show a drop down of suggested locations
        ListView {
            id: suggestView
            height: 300
            width: textField.width
            visible: false
            clip: true
            model: geocodeSuggestions
            delegate: Component {
                Rectangle {
                    id: rect
                    width: parent.width
                    height: 25
                    color: "#f7f8fa"

                    Rectangle {
                        anchors {
                            top: parent.top;
                            left: parent.left;
                            right: parent.right;
                            topMargin: -5
                            leftMargin: 20
                            rightMargin: 20
                        }
                        color: "darkgrey"
                        height: 1
                    }

                    Text {
                        text: name
                        anchors {
                            fill: parent
                            leftMargin: 5
                        }

                        font.pixelSize: 14
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            textField.text = name;
                            suggestView.visible = false;
                            findAddressSample.geocodeAddress(name);
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
