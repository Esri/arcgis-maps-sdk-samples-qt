// [Legal]
// Copyright 2022 Esri.

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
import Esri.ArcGISRuntimeSamples
import Calcite

Rectangle {
    visible: SampleManager.currentMode === SampleManager.DescriptionView
    color: Calcite.background
    property string descriptionText: "text"

    function constrainDescriptionImages() {
        TextDocumentUtils.constrainImageWidths(textEdit.textDocument);
    }

    // Format changes also emit TextEdit.textChanged, so respond to the source property instead.
    onDescriptionTextChanged: Qt.callLater(constrainDescriptionImages)

    Flickable {
        id: descriptionFlickable
        anchors {
            margins: 15
            fill: parent
        }
        contentWidth: textEdit.width
        contentHeight: textEdit.height
        ScrollIndicator.vertical: ScrollIndicator {}

        TextEdit {
            id: textEdit
            width: descriptionFlickable.width
            // Percentage image widths require wrapping to give the document a finite text width.
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            readOnly: true
            activeFocusOnPress: false
            textFormat: Text.MarkdownText
            color: Calcite.text1
            text: descriptionText
            onLinkActivated: Qt.openUrlExternally(link)

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
    }
}
