// [WriteFile Name=CreateAndSaveKmlFile, Category=Layers]
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

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Esri.Samples

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    CreateAndSaveKmlFileSample {
        id: model
        mapView: view

        onKmlSaveCompleted: saveCompleteDialog.open();
    }

    Button {
        anchors{
            left: parent.left
            top: parent.top
            margins: 3
        }
        text: qsTr("Save kmz file")

        onClicked: {
            fileDialog.visible = true;
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        visible: model.busy
    }

    FileDialog {
        id: fileDialog
        defaultSuffix: "kmz"
        fileMode: FileDialog.SaveFile
        nameFilters: ["Kml files (*.kmz *.kml)"]
        onAccepted: {
            // Write the KML document to the chosen path.
            visible: false;
            model.saveKml(currentFile);
        }
        onRejected: {
            visible: false;
        }
    }

    Dialog {
        id: saveCompleteDialog
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        standardButtons: Dialog.Ok
        title: "Item saved."
    }
}
