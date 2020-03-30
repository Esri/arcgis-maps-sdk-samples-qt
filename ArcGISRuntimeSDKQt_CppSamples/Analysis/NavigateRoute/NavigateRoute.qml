// [WriteFile Name=NavigateRoute, Category=Analysis]
// [Legal]
// Copyright 2020 Esri.

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
import Esri.Samples 1.0
import QtQuick.Layouts 1.11

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Rectangle {
            id: backBox
            z: 1
            width: Qt.platform.os === "ios" || Qt.platform.os === "android" ? 200 : 300
            height: 200
            color: "#FBFBFB"
            border.color: "black"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 20


                RowLayout {
                    anchors {
                        top: parent.top
                        horizontalCenter: parent.horizontalCenter
                        margins: 5
                    }
                    Button {
                        text: "Navigate"
                        enabled: model.navigationButtonEnabled
                        onClicked: {
                            model.startNavigation();
                        }
                    }
                    Button {
                        text: "Recenter"
                        enabled: model.navigationButtonEnabled
                    }
                }

                Row {
                    anchors {
                        left: parent.left
                        top: parent.top
                    }
                    Text {
                        text: model.textString
                    }
                }


        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    NavigateRouteSample {
        id: model
        mapView: view
    }
}
