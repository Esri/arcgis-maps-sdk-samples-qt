// [WriteFile Name=SpatialRelationships, Category=Geometry]
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
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import Esri.Samples 1.0

SpatialRelationshipsSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    Rectangle {
        anchors {
            fill: relationshipColumn
            margins: -10
        }
        opacity: 0.85
        radius: 5
        color: "#e2e2e2"
        border {
            color: "darkgray"
            width: 1
        }
    }

    Column {
        id: relationshipColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 15
        }
        spacing: 5

        Text {
            text: "Relationships:"
            font {
                pixelSize: 16
                bold: true
                family: "helvetica"
            }
        }

        Text {
            id: pointText
            visible: text.length > 0
            font.family: "helvetica"
            text: pointRelationships
        }

        Text {
            id: lineText
            visible: text.length > 0
            font.family: "helvetica"
            text: polylineRelationships
        }

        Text {
            id: polygonText
            visible: text.length > 0
            font.family: "helvetica"
            text: polygonRelationships
        }
    }
}
