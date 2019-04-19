// [WriteFile Name=ReadSymbolsFromMobileStyle, Category=DisplayInformation]
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

import QtQuick 2.11
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Button {
            anchors {
                bottom: view.attributionTop
                horizontalCenter: parent.horizontalCenter
                bottomMargin: 10
            }
            text: "Clear Graphics"
            onClicked: model.clearGraphics();
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ReadSymbolsFromMobileStyleSample {
        id: model
        mapView: view
    }

    Rectangle {
        anchors {
            fill: optionGrid
            margins: -5
        }
        color: "#efefef"
    }

    GridLayout {
        id: optionGrid
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }

        columns: 2

        Label {
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignCenter
            text: "Change Options to Modify Symbol"
            font.underline: true
        }

        Label {
            text: "Eyes"
        }

        SymbolComboBox {
            id: eyeComboBox
            model: model.eyesResults
            onCurrentTextChanged: updateSymbol()
        }

        Label {
            text: "Mouth"
        }

        SymbolComboBox {
            id: mouthComboBox
            model: model.mouthResults
            onCurrentTextChanged: updateSymbol()
        }

        Label {
            text: "Hat"
        }

        SymbolComboBox {
            id: hatComboBox
            model: model.hatResults
            onCurrentTextChanged: updateSymbol()
        }

        Label {
            text: "Base Color"
        }

        ComboBox {
            id: colorComboBox
            model: ["yellow", "green", "pink"]
            onCurrentTextChanged: updateSymbol()
        }

        Label {
            text: "Size"
        }

        Slider {
            id: sizeSlider
            Layout.preferredWidth: parent.width / 2
            from: 1
            to: 60
            value: 40
            onValueChanged: updateSymbol()
        }

        Label {
            text: "Preview:"
        }

        Image {
            id: symbolImage
            source: model.symbolImageUrl
            sourceSize.width: 40
            sourceSize.height: 40
            fillMode: Image.PreserveAspectFit
            width: 40
            height: 40
        }
    }

    function updateSymbol() {
        model.updateSymbol(hatComboBox.currentIndex, mouthComboBox.currentIndex, eyeComboBox.currentIndex,
                           colorComboBox.currentText, sizeSlider.value);
    }
}
