// [WriteFile Name=RasterFunctionService, Category=Layers]
// [Legal]
// Copyright 2017 Esri.

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
import QtQuick.Window 2.2
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

RasterFunctionServiceSample {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    
    property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/raster/"

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    // Create a button to apply the raster function
    Rectangle {
        id: button
        property bool pressed: false

        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: 20
            bottomMargin: 40
        }

        width: 45
        height: width
        color: pressed ? "#959595" : "#D6D6D6"
        radius: 100
        border {
            color: "#585858"
            width: 1
        }

        Image {
            anchors {
                centerIn: parent
            }
            width: 26
            height: width
            source: "qrc:/Samples/Layers/RasterFunctionService/RasterImageAnalysisHillshade.png"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: button.pressed = true
            onReleased: button.pressed = false
            onClicked: applyRasterFunction()
        }
    }
}
