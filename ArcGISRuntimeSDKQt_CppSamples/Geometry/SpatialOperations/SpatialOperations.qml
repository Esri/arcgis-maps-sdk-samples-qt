// [WriteFile Name=SpatialOperations, Category=Geometry]
// [Legal]
// Copyright 2018 Esri.

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
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

SpatialOperationsSample {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"        
    }

    Rectangle {
        anchors {
            fill: operationColumn
            margins: -5 * scaleFactor
        }
        radius: 2 * scaleFactor
        color: "lightgray"
        border {
            color: "gray"
            width:  1 * scaleFactor
        }
    }

    Column {
        id: operationColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 10 * scaleFactor
        }

        spacing: 10 * scaleFactor

        ExclusiveGroup {
            id: radiobuttongroup
        }

        // Display a RadioButton for each operation
        Repeater {
            model: geometryOperations

            RadioButton {
                text: modelData
                checked: index === 0
                exclusiveGroup: radiobuttongroup
                onClicked: applyGeometryOperation(index);
            }
        }
    }
}
