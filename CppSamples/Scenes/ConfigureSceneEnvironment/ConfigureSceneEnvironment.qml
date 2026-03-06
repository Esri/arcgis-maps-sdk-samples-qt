// [WriteFile Name=ConfigureSceneEnvironment, Category=Scenes]
// [Legal]
// Copyright 2026 Esri.
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

Item {
  LocalSceneView {
    id: view
    anchors.fill: parent
    Component.onCompleted: {
      forceActiveFocus();
    }
  }

  ConfigureSceneEnvironmentSample {
    id: sceneModel
    localSceneView: view
  }

  // Settings panel
  Rectangle {
    anchors {
      top: parent.top
      right: parent.right
      margins: 10
    }
    width: Math.min(300, parent.width - 20)
    height: Math.min(panelContent.implicitHeight + 20, parent.height * 0.7)
    color: palette.base
    radius: 8
    border.color: palette.dark
    border.width: 1

    MouseArea {
      anchors.fill: parent
      onClicked: mouse => mouse.accepted = true
      onWheel: wheel => wheel.accepted = true
    }

    Flickable {
      id: flickable
      anchors.fill: parent
      anchors.margins: 10
      contentHeight: panelContent.implicitHeight
      clip: true
      boundsBehavior: Flickable.StopAtBounds

      ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AsNeeded
      }

      ColumnLayout {
        id: panelContent
        width: flickable.width
        spacing: 8

        Label {
          text: qsTr("Sky")
          font.bold: true
          font.pixelSize: 14
          color: palette.text
        }

        RowLayout {
          Layout.fillWidth: true
          Label {
            text: qsTr("Stars")
            color: palette.text
            Layout.fillWidth: true
          }
          Switch {
            checked: sceneModel.starsEnabled
            onToggled: sceneModel.starsEnabled = checked
          }
        }

        RowLayout {
          Layout.fillWidth: true
          Label {
            text: qsTr("Atmosphere")
            color: palette.text
            Layout.fillWidth: true
          }
          Switch {
            checked: sceneModel.atmosphereEnabled
            onToggled: sceneModel.atmosphereEnabled = checked
          }
        }

        Rectangle {
          Layout.fillWidth: true
          height: 1
          color: palette.mid
        }

        Label {
          text: qsTr("Background color")
          font.bold: true
          font.pixelSize: 14
          color: palette.text
        }

        ComboBox {
          Layout.fillWidth: true
          model: ListModel {
            ListElement { text: qsTr("None");   value: "transparent" }
            ListElement { text: qsTr("Black");  value: "black" }
            ListElement { text: qsTr("Red");    value: "red" }
            ListElement { text: qsTr("Orange"); value: "orange" }
            ListElement { text: qsTr("Yellow"); value: "yellow" }
            ListElement { text: qsTr("Green");  value: "green" }
            ListElement { text: qsTr("Blue");   value: "blue" }
            ListElement { text: qsTr("Purple"); value: "purple" }
            ListElement { text: qsTr("White");  value: "white" }
          }
          textRole: "text"
          valueRole: "value"
          currentIndex: 0
          onActivated: sceneModel.backgroundColor = currentValue
        }

        Rectangle {
          Layout.fillWidth: true
          height: 1
          color: palette.mid
        }

        Label {
          text: qsTr("Lighting")
          font.bold: true
          font.pixelSize: 14
          color: palette.text
        }

        RowLayout {
          spacing: 10
          RadioButton {
            id: sunRadioButton
            text: qsTr("Sun")
            checked: sceneModel.sunLighting
            onToggled: {
              if (checked) {
                sceneModel.sunLighting = true
              }
            }
          }
          RadioButton {
            text: qsTr("Virtual")
            checked: !sceneModel.sunLighting
            onToggled: {
              if (checked) {
                sceneModel.sunLighting = false;
              }
            }
          }
        }

        RowLayout {
          Layout.fillWidth: true
          Label {
            text: qsTr("Direct Shadows")
            color: palette.text
            Layout.fillWidth: true
          }
          Switch {
            checked: sceneModel.directShadowsEnabled
            onToggled: sceneModel.directShadowsEnabled = checked
          }
        }

        RowLayout {
          Layout.fillWidth: true
          spacing: 5
          enabled: sunRadioButton.checked
          Label {
            text: qsTr("Hour: %1:00").arg(Math.floor(hourSlider.value))
            color: palette.text
            Layout.minimumWidth: 75
          }
          Slider {
            id: hourSlider
            Layout.fillWidth: true
            from: 0
            to: 23
            value: 12
            stepSize: 1
            onMoved: sceneModel.lightingHour = value
          }
        }
      }
    }
  }
}
