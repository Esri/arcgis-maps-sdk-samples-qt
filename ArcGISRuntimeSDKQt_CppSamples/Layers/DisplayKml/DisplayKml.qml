// [WriteFile Name=DisplayKml, Category=Layers]
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

import QtQuick
import QtQuick.Controls
import Esri.Samples

DisplayKmlSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    SceneView {
        objectName: "sceneView"
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on SceneView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    ComboBox {
        id: comboBox
        anchors {
            right: parent.right
            top: parent.top
            margins: 5
        }

        // Add a background to the ComboBox
        Rectangle {
            anchors.fill: parent
            radius: 10
            // Make the rectangle visible if a dropdown indicator exists
            // An indicator only exists if a theme is set
            visible: parent.indicator
            border.width: 1
        }

        property int modelWidth: 0
<<<<<<< HEAD
        width: modelWidth + leftPadding + rightPadding + (indicator ? indicator.width : 10)
=======
        width: modelWidth + leftPadding + rightPadding
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd

        model: ["URL", "Local file", "Portal Item"]

        onCurrentIndexChanged: {
            // create the Layer
            if (currentIndex === 0)
                createFromUrl();
            else if (currentIndex === 1)
                createFromFile();
            else
                createFromPortalItem();
        }

        Component.onCompleted : {
            for (let i = 0; i < model.length; ++i) {
                metrics.text = model[i];
                modelWidth = Math.max(modelWidth, metrics.width);
            }
            currentIndexChanged();
        }

        TextMetrics {
            id: metrics
            font: comboBox.font
        }
    }
}
