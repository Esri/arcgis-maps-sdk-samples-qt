// [WriteFile Name=DownloadPreplannedMap, Category=Maps]
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

import QtQuick 2.6
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Esri.Samples 1.0

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    DownloadPreplannedMapSample {
        id: model
        mapView: view
    }

    Rectangle {
        id: buttonBackground
        anchors {
            left: parent.left
            top: parent.top
            margins: 3
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#000000"
        opacity: .8
        radius: 5

        // catch mouse signals from propagating to parent
        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        ColumnLayout {
            Button {
                Layout.fillWidth: true
                Layout.margins: 2
                Layout.alignment: Qt.AlignHCenter
                enabled: !busy.visible & !model.viewingOnlineMaps
                text: qsTr("Show Online Map")
                onClicked: model.showOnlineMap(preplannedCombo.currentIndex);
            }

            Text {
                text: qsTr("Available Preplanned Areas:")
                color: "white"
                Layout.alignment: Qt.AlignHCenter
            }

            ComboBox {
                id: preplannedCombo
                Layout.fillWidth: true
                Layout.margins: 2
                enabled: !busy.visible
                model: model.preplannedModel
                textRole: "itemTitle"
                onActivated: model.checkIfMapExists(preplannedCombo.currentIndex);
            }

            Button {
                id: downloadOrView
                Layout.fillWidth: true
                Layout.margins: 2
                enabled: !busy.visible
                text: model.preplannedMapExists ? qsTr("View preplanned area") : qsTr("Download preplanned area")
                onClicked: model.checkIfMapAreaIsLoaded(preplannedCombo.currentIndex);
            }

            Text {
                text: qsTr("Download(s) deleted on exit")
                color: "white"
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 2
            }
        }
    }

    BusyIndicator {
        id: busy
        anchors.centerIn: parent
        visible: model.busy
    }
}
