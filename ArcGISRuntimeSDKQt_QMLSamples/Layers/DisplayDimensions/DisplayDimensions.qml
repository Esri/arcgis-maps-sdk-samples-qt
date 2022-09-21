// [WriteFile Name=DisplayDimensions, Category=Layers]
// [Legal]
// Copyright 2021 Esri.

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

import Qt.labs.platform
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Esri.ArcGISRuntime
import Esri.ArcGISExtras

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/mmpk/"
    property string errorMessage: ""
    property int indexOfDimensionLayer: 0

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        Control {
            id: toggleControl
            anchors {
                top: mapView.top
                topMargin: 10
                left: mapView.left
                leftMargin: 10
            }
            background: Rectangle {
                color: "white"
                border.color: "black"
            }
            padding: 5

            contentItem: GridLayout {
                columns: 1
                Text {
                    id: toggleBoxTitle
                }
                CheckBox {
                    id: visibilityToggle
                    text: "Dimension Layer visibility"
                    checked: true
                    onCheckStateChanged: mmpk.maps[0].operationalLayers.get(indexOfDimensionLayer).visible = visibilityToggle.checkState;
                    enabled: mmpk.loadStatus === Enums.LoadStatusLoaded ? true : false
                }
                CheckBox {
                    id: definitionExpressionToggle
                    text: "Definition Expression: \nDimensions >= 450m"
                    checked: false
                    onCheckStateChanged: {
                        if (definitionExpressionToggle.checkState === Qt.Checked) {
                            mmpk.maps[0].operationalLayers.get(indexOfDimensionLayer).definitionExpression = "DIMLENGTH >= 450";
                        }
                        else {
                            mmpk.maps[0].operationalLayers.get(indexOfDimensionLayer).definitionExpression = "";
                        }
                    }
                    enabled: mmpk.loadStatus === Enums.LoadStatusLoaded && visibilityToggle.checked ? true : false
                }
            }
        }

        // Pop-up error message box
        MessageDialog {
            id: errorMessageBox
            text: errorMessage
            visible: errorMessage === "" ? false : true;
            onAccepted: errorMessage = "";
        }
    }

    Component.onCompleted: mmpk.load();

    MobileMapPackage {
        id: mmpk
        path: dataPath + "Edinburgh_Pylon_Dimensions.mmpk"

        onLoadStatusChanged: {
            // If an error occurred during loading, pass the error onto the handleError function.
            if (loadStatus === Enums.LoadStatusFailedToLoad) {
                handleError(mmpk.loadError);
            }

            // If the mmpk has not loaded, return.
            if (loadStatus !== Enums.LoadStatusLoaded) {
                return;
            }

            // If there is more than one map, return.
            if (mmpk.maps.length > 1) {
                return;
            }

            // Set the map view's map to the first map in the mobile map package
            mapView.map = mmpk.maps[0];
            // Set the minimum scale as 1:35000 to prevent zooming out too far.
            mapView.map.minScale = 35000;

            // Loop through all layers in the mmpk and find the dimension layer.
            for (let counter = 0; counter < mmpk.maps[0].operationalLayers.count; counter++) {
                // Check each layer to see if it is a DimensionLayer.
                if (mmpk.maps[0].operationalLayers.get(counter).layerType === Enums.LayerTypeDimensionLayer) {
                    // Save the index of the DimensionLayer - it is used to control layer visibility and definition expressions.
                    indexOfDimensionLayer = counter;
                    // Use the name of the DimensionLayer to define the title of the UI controls.
                    toggleBoxTitle.text = mmpk.maps[0].operationalLayers.get(counter).name;
                    // There is only one Dimension Layer, so we can break out of the loop.
                    break;
                }
            }

        }
    }

    function handleError(error) {
        if (!error.additionalMessage)
            errorMessage = error.message;
        else
            errorMessage = error.message + "\n" + error.additionalMessage;
    }
}
