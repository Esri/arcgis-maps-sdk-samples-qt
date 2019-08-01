// [WriteFile Name=PlayAKmlTour, Category=Layers]
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

    SceneView {
        id: view
        anchors.fill: parent
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
        opacity: .75
        radius: 5

        // catch mouse signals from propagating to parent
        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        RowLayout {
            spacing: 0
            Button {
                id: playButton
                text: qsTr("Play")
                Layout.margins: 2
                enabled: playAKmlTourSampleModel.playButtonEnabled
                onClicked: playAKmlTourSampleModel.playKmlTour();
            }

            Button {
                id: pauseButton
                text: qsTr("Pause")
                Layout.margins: 2
                enabled: playAKmlTourSampleModel.pauseButtonEnabled
                onClicked: playAKmlTourSampleModel.pauseKmlTour();
            }

            Button {
                id: resetButton
                text: qsTr("Reset")
                Layout.margins: 2
                enabled: playAKmlTourSampleModel.resetButtonEnabled
                onClicked: playAKmlTourSampleModel.resetKmlTour();
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    PlayAKmlTourSample {
        id: playAKmlTourSampleModel
        sceneView: view
    }
}
