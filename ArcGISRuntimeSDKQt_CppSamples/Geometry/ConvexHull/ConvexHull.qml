// [WriteFile Name=ConvexHull, Category=Geometry]
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
import QtQuick.Controls 2.6
import Esri.Samples 1.0

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Rectangle {
            id: buttonBackground
            anchors {
                top: parent.top
                left: parent.left
                margins: 2
//                color: "white"
            }

            width: 80 //childrenRect.width
            height: 20 //childrenRect.height

            Button {
                id: hullButton
                width: parent.width
                height: parent.height
                Text {
                    text: "Convex Hull"
                }
                onClicked: {
                    model.displayConvexHull();
                }
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ConvexHullSample {
        id: model
        mapView: view
    }
}
