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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

StatisticalQuerySample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600
    
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
            margins: -10
        }
        color: "#E9E9EA"
        radius: 3
        opacity: 0.8
        border {
            color: "black"
            width: 1
        }
    }

    Column {
        id: controlColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 15
        }
        spacing: 5

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

    Dialog {
        id: msgDialog
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        property alias text : textLabel.text
        property alias informativeText : detailsLabel.text
        ColumnLayout {
            Text {
                id: textLabel
                text: "Query Statistics Results:"
            }
            Text {
                id: detailsLabel
            }
        }
    }

    onShowStatistics: {
        msgDialog.informativeText = results;
        msgDialog.open();
    }
}
