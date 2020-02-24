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
import QtQuick.Layouts 1.11

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        RowLayout {
            id: layout
//            anchors.fill: parent
            anchors {
             left: parent.left
             top: parent.top
            }
        // create a button to create and show the convex hull
        Button {
            id: hullButton
//            anchors {
//                left: parent.left
//                top: parent.top
//                margins: 5
//            }
            width: implicitWidth
            height: implicitHeight
            text: "Convex Hull"
            onClicked: {
                model.displayConvexHull();
            }
        }

        Button {
            id: clearButton
            width: implicitWidth
            height: implicitHeight
            text: "Reset"
            onClicked: {
                model.clearGraphics();
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
