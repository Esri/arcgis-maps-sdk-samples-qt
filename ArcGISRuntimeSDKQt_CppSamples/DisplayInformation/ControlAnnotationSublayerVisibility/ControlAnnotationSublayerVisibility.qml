// [WriteFile Name=ControlAnnotationSublayerVisibility, Category=DisplayInformation]
// [Legal]
// Copyright 2019 Esri.

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
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.2
import Esri.Samples 1.0

Item {
    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Rectangle {
            id: checkBoxBackground
            anchors {
                left: parent.left
                top: parent.top
                margins: 2
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "white"
            opacity: .75
            radius: 5

            ColumnLayout {
                spacing: 0
                Row {
                    CheckBox {
                        id: openBox
                        checked: true
                        onCheckStateChanged: controlAnnotationSublayerVisibilityModel.openLayerVisible();
                    }

                    Text {
                        id: openBoxText
                        text: controlAnnotationSublayerVisibilityModel.openLayerText
                        anchors.verticalCenter: openBox.verticalCenter
                        color: scale.color
                    }
                }

                Row {
                    CheckBox {
                        id: closedBox
                        checked: true
                        onCheckStateChanged: controlAnnotationSublayerVisibilityModel.closedLayerVisible();
                    }

                    Text {
                        id: closedBoxText
                        text: controlAnnotationSublayerVisibilityModel.closedLayerText
                        anchors.verticalCenter: closedBox.verticalCenter
                    }
                }
            }
        }

        Rectangle {
            id: currentScale
            anchors {
                bottom: view.attributionTop
                horizontalCenter: parent.horizontalCenter
            }
            width: childrenRect.width
            height: childrenRect.height

            Text {
                id: scale
                text: "Current map scale: 1:%1".arg(Math.round(controlAnnotationSublayerVisibilityModel.mapScale))
                color: controlAnnotationSublayerVisibilityModel.visibleAtCurrentExtent ? "black" : "grey"
                padding: 2
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ControlAnnotationSublayerVisibilitySample {
        id: controlAnnotationSublayerVisibilityModel
        mapView: view
    }
}
