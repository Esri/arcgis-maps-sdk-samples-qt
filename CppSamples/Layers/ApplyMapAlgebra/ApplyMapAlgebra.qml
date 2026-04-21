// [WriteFile Name=ApplyMapAlgebra, Category=Layers]
// [Legal]
// Copyright 2026 Esri.

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
import QtQuick.Layouts
import Esri.Samples

ApplyMapAlgebraSample {
  id: root

  // add a mapView component
  MapView {
    id: view
    anchors.fill: parent
    objectName: "mapView"

    Component.onCompleted: {
      // Set and keep the focus on MapView to enable keyboard navigation
      forceActiveFocus();
    }
  }

  Rectangle {
    visible: !resultsAvailable
    anchors {
      top: parent.top
      right: parent.right
      margins: 16
    }
    radius: 8
    color: palette.base
    border.color: palette.dark

    width: Math.min(280, parent.width - 32)
    implicitHeight: actionColumn.implicitHeight + 24

    MouseArea {
      anchors.fill: parent
      onClicked: mouse => mouse.accepted = true
      onWheel: wheel => wheel.accepted = true
    }

    ColumnLayout {
      id: actionColumn
      anchors.fill: parent
      anchors.margins: 12
      spacing: 8

      Label {
        text: qsTr("Create geomorphic categories from the Arran elevation raster.")
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
        color: palette.text
      }

      Button {
        text: busy ? qsTr("Map algebra computing...") : qsTr("Categorize")
        enabled: dataAvailable && !busy
        onClicked: categorize()
      }
    }
  }

  Rectangle {
    visible: resultsAvailable
    anchors {
      top: parent.top
      right: parent.right
      margins: 16
    }
    radius: 8
    color: palette.base
    border.color: palette.dark

    width: Math.min(280, parent.width - 32)
    height: resultsColumn.implicitHeight + 24

    MouseArea {
      anchors.fill: parent
      onClicked: mouse => mouse.accepted = true
      onWheel: wheel => wheel.accepted = true
    }

    ColumnLayout {
      id: resultsColumn
      anchors.fill: parent
      anchors.margins: 12
      spacing: 8

      Label {
        text: qsTr("Display")
        font.bold: true
        color: palette.text
      }

      RadioButton {
        text: qsTr("Map algebra results")
        checked: showGeomorphicResults
        onToggled: if (checked) showGeomorphicResults = true
      }

      RadioButton {
        text: qsTr("Original elevation raster")
        checked: !showGeomorphicResults
        onToggled: if (checked) showGeomorphicResults = false
      }
    }
  }

  Rectangle {
    visible: errorMessage.length > 0
    anchors {
      left: parent.left
      right: parent.right
      bottom: attributionLabel.top
      margins: 16
      bottomMargin: 12
    }
    radius: 8
    color: Qt.rgba(0.35, 0.12, 0.12, 0.92)
    border.color: Qt.rgba(1, 1, 1, 0.15)
    implicitHeight: errorLabel.implicitHeight + 20

    Label {
      id: errorLabel
      anchors.fill: parent
      anchors.margins: 10
      text: errorMessage
      wrapMode: Text.WordWrap
      color: "white"
    }

    MouseArea {
      anchors.fill: parent
      onClicked: mouse => mouse.accepted = true
      onWheel: wheel => wheel.accepted = true
    }
  }

  Label {
    id: attributionLabel
    anchors {
      right: parent.right
      left: parent.left
      bottom: parent.bottom
      leftMargin: 16
      rightMargin: 4
      bottomMargin: 32
    }
    horizontalAlignment: Text.AlignRight
    wrapMode: Text.WordWrap
    text: qsTr("Raster data Copyright Scottish Government and SEPA (2014)")
    color: Qt.rgba(1, 1, 1, 0.78)
    font.pixelSize: 11
    font.italic: true
  }
}
