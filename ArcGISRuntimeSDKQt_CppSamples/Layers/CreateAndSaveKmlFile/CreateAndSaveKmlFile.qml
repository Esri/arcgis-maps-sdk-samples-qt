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

import QtQuick 2.6
import QtQuick.Controls 2.2
import Qt.labs.platform 1.0
import Esri.Samples 1.0

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    CreateAndSaveKmlFileSample {
        id: model
        mapView: view
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
        onAccepted: {
            visible: false;
            model.saveKml(currentFile);
        }
        onRejected: {
            visible: false;
        }
    }
}
