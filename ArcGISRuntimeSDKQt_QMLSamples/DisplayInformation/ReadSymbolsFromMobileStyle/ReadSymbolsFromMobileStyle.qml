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
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url styleDataPath: System.userHomePath + "/ArcGIS/Runtime/Data/styles/emoji-mobile.stylx"
    property var currentSymbol
    property var currentFace
    property var currentEyes
    property var currentMouth
    property var currentHat

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapTopographic {}
        }

        GraphicsOverlay {
            id: graphicsOverlay
        }

        onMouseClicked: {
            const graphic = ArcGISRuntimeEnvironment.createObject("Graphic");
            graphic.geometry = mouse.mapPoint;
            graphic.symbol = currentSymbol;
            graphicsOverlay.graphics.append(graphic);
        }

        Button {
            anchors {
                bottom: mapView.attributionTop
                horizontalCenter: parent.horizontalCenter
                bottomMargin: 10
            }
            text: "Clear Graphics"
            onClicked: graphicsOverlay.graphics.clear();
        }
    }

    SymbolStyle {
        id: symbolStyle
        styleLocation: styleDataPath
        Component.onCompleted: load()

        onLoadStatusChanged: {
            // fetch the default parameters
            if (loadStatus === Enums.LoadStatusLoaded) {
                defaultSearchParameters();
            }
        }

        onDefaultSearchParametersStatusChanged: {
            // search the symbol style with defaults - all symbols will be returned
            if (defaultSearchParametersStatus === Enums.TaskStatusCompleted) {
                searchSymbols(defaultSearchParametersResult);
            }
        }

        onSearchSymbolsStatusChanged: {
            if (searchSymbolsStatus !== Enums.TaskStatusCompleted)
                return;

            // get initial list of symbols
            for (let i = 0; i < searchSymbolsResult.count; i++) {
                const result = searchSymbolsResult.get(i);
                if (result.category === "Face") {
                    currentFace = result;
                    break;
                }
            }

            // fetch a new symbol with the initial symbol layer keys
            updateSymbol();
        }

        onFetchSymbolStatusChanged: {
            if (fetchSymbolStatus !== Enums.TaskStatusCompleted)
                return;

            // set the current symbol
            currentSymbol = fetchSymbolResult;

            // set the color locked preferences per layer
            currentSymbol.symbolLayers.forEach(symbolLyr => symbolLyr.colorLocked = true);

            currentSymbol.symbolLayers.get(0).colorLocked = false;

            // update size
            currentSymbol.size = sizeSlider.value;

            // update color
            currentSymbol.color = colorComboBox.currentText;

            // update swatch
            currentSymbol.swatchImageChanged.connect(()=> {
                symbolImage.source = currentSymbol.swatchImage;
            });

            currentSymbol.createSwatch();
        }
    }

    function updateSymbol() {
        if (!currentFace || !currentEyes || !currentMouth || !currentHat)
            return;

        // fetch the multilayer symbol composed of these 4 symbol layers
        symbolStyle.fetchSymbolWithKeyList([currentFace.key, currentEyes.key, currentMouth.key, currentHat.key]);
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
            dataPath: styleDataPath
            filterKeyword: "Eyes"
            onSelectionCompleted: {
                currentEyes = currentItem;
                updateSymbol();
            }
        }

        Label {
            text: "Mouth"
        }

        SymbolComboBox {
            dataPath: styleDataPath
            filterKeyword: "Mouth"
            onSelectionCompleted: {
                currentMouth = currentItem;
                updateSymbol();
            }
        }

        Label {
            text: "Hat"
        }

        SymbolComboBox {
            dataPath: styleDataPath
            filterKeyword: "Hat"
            onSelectionCompleted: {
                currentHat = currentItem;
                updateSymbol();
            }
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
            sourceSize.width: 40
            sourceSize.height: 40
            fillMode: Image.PreserveAspectFit
            width: 40
            height: 40
        }
    }
}
