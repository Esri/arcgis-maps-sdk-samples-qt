// [WriteFile Name=StatisticalQuery, Category=Analysis]
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
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

StatisticalQuerySample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property alias extentOnly: extentCheckbox.checked
    property alias bigCitiesOnly: bigCitiesCheckbox.checked
    property string resultsText: ""

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    Rectangle {
        anchors {
            fill: controlColumn
            margins: -10 * scaleFactor
        }
        color: "#E9E9EA"
        radius: 3 * scaleFactor
        opacity: 0.8
        border {
            color: "black"
            width: 1 * scaleFactor
        }
    }

    Column {
        id: controlColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 15 * scaleFactor
        }
        spacing: 5 * scaleFactor

        CheckBox {
            id: extentCheckbox
            text: "Only cities in current extent"
        }

        CheckBox {
            id: bigCitiesCheckbox
            text: "Only cities greater than 5M"
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Get Statistics"
            onClicked: queryStatistics(extentOnly, bigCitiesOnly)
        }
    }

    // Create a dialog to display the result
    MessageDialog {
        id: msgDialog
        text: "Query Statistics Results:"
    }

    onShowStatistics: {
        msgDialog.open();
        msgDialog.informativeText = results;
    }
}
