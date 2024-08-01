// [WriteFile Name=CreateDynamicBasemapGallery, Category=Maps]
// [Legal]
// Copyright 2024 Esri.
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
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import Esri.Samples

Item {
    property real flowCellWidth: 200

    // Create MapQuickView here, and create its Map etc. in C++ code
    MapView {
        id: view
        anchors.fill: parent
        // set focus to enable keyboard navigation
        focus: true

        RoundButton {
            x: 10
            y: 10
            icon.name: "grid-24"
            icon.source: "grid-24.svg"
            onClicked: popup.open()
            }

            Popup {
                id: popup
                x: parent.width * 0.1
                y: parent.height * 0.1
                width: parent.width * 0.8
                height: parent.height * 0.8
                modal: true
                focus: true
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

                ColumnLayout {
                    anchors.fill: parent
                    clip: true

                    GridView {
                        id: basemapView
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: false
                        Layout.fillHeight: true
                        implicitWidth: {
                            var numCellsInPopup_dbl = popup.width / basemapView.cellWidth;
                            var numCellsInPopup_int = Math.trunc(numCellsInPopup_dbl);
                            return Math.max(1, numCellsInPopup_int) * basemapView.cellWidth;
                        }
                        clip: true
                        model: model.gallery
                        currentIndex: model.indexOfSelectedStyle;
                        cellWidth: 200
                        cellHeight: 160
                        highlight: Rectangle {
                            width: basemapView.cellWidth;
                            height: basemapView.cellHeight
                            color: "lightsteelblue";
                            anchors.fill: basemapView.currentItem
                        }
                        highlightFollowsCurrentItem: false;

                        delegate: ItemDelegate {
                            id: basemapDelegate
                            width: basemapView.cellWidth
                            height: basemapView.cellHeight
                            required property string styleName
                            required property string previewImageUrl
                            onClicked: {
                                model.updateSelectedStyle(styleName);
                            }
                            clip: true
                            Column {
                                spacing: 5
                                padding: 10
                                Text {
                                    font.bold: true
                                    font.pixelSize: 12
                                    text: styleName + ":"
                                }
                                Image {
                                    source: previewImageUrl
                                    fillMode: Image.PreserveAspectFit
                                    width: basemapView.cellWidth - 25
                                }
                            }
                        }
                    }

                    Flow {
                        id: flow
                        Layout.fillWidth: false
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: calculateFlowLayoutWidth();
                        spacing: 10

                        ColumnLayout {
                            Text {
                                Layout.preferredHeight: 15
                                font.underline: true
                                text: "Language Strategy:"
                            }
                            ComboBox {
                                id: languageStrategyOptions
                                Layout.preferredHeight: 30
                                Layout.preferredWidth: flowCellWidth
                                model: model.languageStrategies
                                enabled: model.languageStrategies.length !== 0
                            }
                        }
                        ColumnLayout {
                            Text {
                                Layout.preferredHeight: 15
                                font.underline: true
                                text: "Language:"
                            }
                            ComboBox {
                                id: languages
                                Layout.preferredHeight: 30
                                Layout.preferredWidth: flowCellWidth
                                model: model.languages
                                enabled: model.languages.length !== 0
                            }
                        }
                        ColumnLayout {
                            Text {
                                Layout.preferredHeight: 15
                                font.underline: true
                                text: "Worldview:"
                            }
                            ComboBox {
                                id: worldviews
                                Layout.preferredHeight: 30
                                Layout.preferredWidth: flowCellWidth
                                model: model.worldviews
                                enabled: model.worldviews.length !== 0
                            }
                        }
                        RowLayout {
                            width: flowCellWidth
                            height: 45
                            Button {
                                Layout.alignment: Qt.AlignCenter
                                text: "Load"
                                Layout.preferredHeight: 40
                                Layout.preferredWidth: 80
                                onPressed: {
                                    model.loadBasemap(
                                               languageStrategyOptions.currentText,
                                               languages.currentText,
                                               worldviews.currentText);
                                    popup.close();
                                }
                            }
                        }
                    }
                }
            }
        }

    // Declare the C++ instance which creates the map etc. and supply the view
    CreateDynamicBasemapGallerySample {
        id: model
        mapView: view
    }

    function calculateFlowLayoutWidth() {
        var widthOfMargins = 2 * flow.spacing;
        var numHorizontalInputs_dbl = (popup.width - widthOfMargins) / (flowCellWidth + flow.spacing);
        var numHorizontalInputs_int = Math.trunc(numHorizontalInputs_dbl);
        var widthOfInputElements = Math.max(1, numHorizontalInputs_int) * flowCellWidth;
        var widthOfSpacing = ((Math.max(1, numHorizontalInputs_int) + 2) * flow.spacing);
        return widthOfInputElements + widthOfSpacing;
    }
}
