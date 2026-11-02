// [WriteFile Name=FilterMosaicDatasetRasterByTimeExtent, Category=DisplayInformation]
// [Legal]
// Copyright 2026 Esri.

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

import QtQuick
import QtQuick.Controls
import Esri.Samples

Item {
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        // Date stepper and opacity controls
        Pane {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: view.attributionTop
                bottomMargin: 10
            }
            padding: 8
            background: Rectangle {
                radius: 5
                color: palette.base
                border {
                    width: 1
                    color: "darkgrey"
                }
            }

            Column {
                spacing: 5

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter

                    ToolButton {
                        icon.source: "qrc:/Samples/DisplayInformation/FilterMosaicDatasetRasterByTimeExtent/chevron-start-24.svg"
                        enabled: model.currentIndex > 0
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("First")
                        onClicked: model.currentIndex = 0
                    }

                    ToolButton {
                        icon.source: "qrc:/Samples/DisplayInformation/FilterMosaicDatasetRasterByTimeExtent/chevron-left-24.svg"
                        enabled: model.currentIndex > 0
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Previous")
                        onClicked: model.currentIndex = model.currentIndex - 1
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        width: 100
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.Wrap
                        text: model.loading ? qsTr("Loading...")
                              : model.loadError.length > 0 ? qsTr("Offline data error")
                              : model.timestampCount > 0 ? model.currentTimestamp : qsTr("No timestamps")
                    }

                    ToolButton {
                        icon.source: "qrc:/Samples/DisplayInformation/FilterMosaicDatasetRasterByTimeExtent/chevron-right-24.svg"
                        enabled: model.currentIndex < model.timestampCount - 1
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Next")
                        onClicked: model.currentIndex = model.currentIndex + 1
                    }

                    ToolButton {
                        icon.source: "qrc:/Samples/DisplayInformation/FilterMosaicDatasetRasterByTimeExtent/chevron-end-24.svg"
                        enabled: model.currentIndex < model.timestampCount - 1
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Last")
                        onClicked: model.currentIndex = model.timestampCount - 1
                    }
                }

                Item {
                    width: opacitySlider.width
                    height: opacityLabel.height

                    Label {
                        id: opacityLabel
                        text: qsTr("Opacity")
                    }

                    Label {
                        anchors.right: parent.right
                        text: qsTr("%1%").arg(Math.round(opacitySlider.value * 100))
                    }
                }

                Slider {
                    id: opacitySlider
                    width: 280
                    enabled: !model.loading && model.loadError.length === 0 && model.timestampCount > 0
                    from: 0
                    to: 1
                    stepSize: 0.01
                    value: 1
                    onValueChanged: model.setRasterOpacity(value)
                }

                Label {
                    width: opacitySlider.width
                    visible: model.loadError.length > 0
                    text: model.loadError
                    textFormat: Text.PlainText
                    wrapMode: Text.Wrap
                    Accessible.name: text
                }
            }
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    FilterMosaicDatasetRasterByTimeExtentSample {
        id: model
        mapView: view
    }
}
