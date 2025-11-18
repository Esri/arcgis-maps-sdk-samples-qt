// [WriteFile Name=AnalyzeViewshed, Category=Analysis]
// [Legal]
// Copyright 2016 Esri.
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
import QtQuick.Layouts
import Esri.Samples

AnalyzeViewshedSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        anchors.fill: parent
        objectName: "mapView"

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    onDisplayErrorDialog: {
        messageDialog.title = qsTr("Error");
        messageDialog.text = titleText;
        messageDialog.detailedText = detailedText;
        messageDialog.open();
    }

    // Create rectangle to display the status
    Rectangle {
        anchors {
            margins: -10
            fill: statusColumn
        }
        color: palette.base
        radius: 5
        border.color: "black"
        opacity: 0.85
    }

    Column {
        id: statusColumn
        anchors {
            right: parent.right
            top: parent.top
            margins: 20
        }

        Label {
            anchors.margins: 5
            visible: !viewshedInProgress
            text: qsTr("Click map to execute viewshed analysis")
            font.pixelSize: 12
        }

        Row {
            anchors.margins: 5
            visible: viewshedInProgress
            spacing: 10

            BusyIndicator {
                anchors.verticalCenter: parent.verticalCenter
                width: 32
                height: width
            }

            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: statusText
                font.pixelSize: 12
            }
        }
    }

    MouseArea {
        anchors.fill: statusColumn
        onClicked: mouse => mouse.accepted = true
        onWheel: wheel => wheel.accepted = true
    }

    Dialog {
        id: messageDialog
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        title: qsTr("Error")
        property alias text : textLabel.text
        property alias detailedText : detailsLabel.text
        ColumnLayout {
            Label {
                id: textLabel
                text: qsTr("Executing geoprocessing failed.")
            }
            Label {
                id: detailsLabel
            }
        }
    }
}
