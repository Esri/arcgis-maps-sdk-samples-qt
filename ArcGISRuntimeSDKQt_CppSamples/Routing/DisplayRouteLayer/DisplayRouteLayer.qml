// [WriteFile Name=DisplayRouteLayer, Category=Routing]
// [Legal]
// Copyright 2022 Esri.

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

import QtQuick 2.12
import QtQuick.Controls 2.12
import Esri.Samples 1.0
import QtQuick.Layouts 1.11

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on SceneView to enable keyboard navigation
            forceActiveFocus();
        }

        Button {
            text: "Directions"
            anchors.bottom: parent.bottom
            anchors.margins: 40
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                popup.open()
                if (popup.opened)
                {
                    popup.contentData = model.getDirections();
                }
            }
        }

        Popup {
            id: popup
            anchors.centerIn: Overlay.overlay
            width: 300
            height: 270
            focus: true
            contentItem: Text {
                text: "\"Start at Portland, OR, USA\"
                    \n\"Keep right to merge onto the highway toward Ross Is. BR. (US-26) / Lake Oswego (OR-43)\"
                    \n\"Continue forward on Ross Island Brg\"
                    \n\"Take the exit on the right to merge onto OR-224 E toward Estacada\"
                    \n\"Keep left at the fork toward Salem (I-5 S)\"
                    \n\"Take exit 260A on the right to merge onto OR-99E-BR / Salem Parkway\"
                    \n\"At the traffic light, turn right on Commercial St SE\"
                    \n\"Finish at Salem, OR, USA, on the left\"" /*+ qsTr(model.getDirections())*/;

                wrapMode: Text.WordWrap

            }
            opacity: .9
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    DisplayRouteLayerSample {
        id: model
        mapView: view
    }
}
