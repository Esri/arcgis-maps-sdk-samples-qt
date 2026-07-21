// [WriteFile Name=NavigateMapViewAndIdentifyFeaturesWithKeyboard, Category=Accessibility]
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
import Esri.ArcGISRuntime.Toolkit
import Esri.Samples

Item {
  id: root

  readonly property real spacing: 12
  readonly property real areaSize: Math.max(0, Math.min(200, width - spacing * 2,
                                                        mapContainer.height - spacing * 2))

  Item {
    id: mapContainer
    anchors.fill: parent

    MapView {
      id: view
      anchors.fill: parent
      focus: true

      Keys.priority: Keys.BeforeItem
      Keys.onPressed: event => {
        if (event.key === Qt.Key_Escape) {
          if (view.calloutData.visible)
            model.dismissCallout();
          event.accepted = true;
        } else if (event.key >= Qt.Key_1 && event.key <= Qt.Key_9) {
          event.accepted = model.showCallout(event.key - Qt.Key_0);
        }
      }

      Component.onCompleted: forceActiveFocus()
    }

    Rectangle {
      anchors.centerIn: parent
      visible: !view.calloutData.visible
      width: root.areaSize
      height: root.areaSize
      color: "transparent"
      border {
        color: "black"
        width: 2
      }
      radius: 4
    }
  }

  Pane {
    id: instructions
    anchors {
      top: parent.top
      right: parent.right
      margins: root.spacing
    }
    width: Math.max(0, Math.min(400, root.width - root.spacing * 2))
    padding: root.spacing

    Column {
      width: parent.width
      spacing: 6

      Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: qsTr("Pan and zoom to bring restaurants into the area of interest. Press 1-9 for details. Press Escape to dismiss.")
      }

      Label {
        width: parent.width
        visible: model.overflowVisible
        wrapMode: Text.Wrap
        color: "#be185d"
        text: qsTr("Too many features in the area. Zoom in to see fewer.")
      }

      Label {
        width: parent.width
        visible: model.errorMessage.length > 0
        wrapMode: Text.Wrap
        textFormat: Text.PlainText
        color: "red"
        text: model.errorMessage
      }
    }
  }

  NavigateMapViewAndIdentifyFeaturesWithKeyboardSample {
    id: model
    mapView: view
    areaOfInterestSize: root.areaSize
  }

  Callout {
    z: 1
    calloutData: view.calloutData
    accessoryButtonVisible: false
    leaderPosition: Callout.LeaderPosition.Automatic
  }
}
