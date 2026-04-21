// [WriteFile Name=EditGeometriesWithProgrammaticReticleTool, Category=EditData]
// [Legal]
// Copyright 2025 Esri.
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

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
	id: settingsPopup
	property var reticleModel
	x: (parent.width - width) / 2
	y: (parent.height - height) / 2
	width: 200
	height: 280
	modal: true
	focus: true
	background: Rectangle { color: palette.base; radius: 10 }
	contentItem: ColumnLayout {
		spacing: 10
		anchors.fill: parent

		// Spacer
		Item {
			height: 3
		}

		ColumnLayout {
			Layout.alignment: Qt.AlignCenter
			enabled: !reticleModel.geometryEditorStarted

			Label {
				text: qsTr("Geometry Type:")
				font.pixelSize: 14
				Layout.alignment: Qt.AlignCenter
			}

			ComboBox {
				id: geometryTypePicker
				Layout.preferredWidth: 140
				model: [qsTr("Point"), qsTr("Multipoint"), qsTr("Polygon"), qsTr("Polyline")]
				onCurrentIndexChanged: reticleModel.selectedGeometryType = currentIndex
				background: Rectangle {
					color: palette.mid
					border.color: "#888"
					border.width: 1
				}
				contentItem: Label {
					text: qsTr(geometryTypePicker.currentText)
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
				}
				Layout.alignment: Qt.AlignCenter
			}
		}

		Rectangle {
			height: 1
			Layout.fillWidth: true
			color: palette.mid
		}

		RowLayout {
			Layout.alignment: Qt.AlignCenter

			Label {
				text: qsTr("Allow Vertex\n Creation:")
				font.pixelSize: 14
			}

			Switch {
				width: 40
				height: 20
				checked: reticleModel.vertexCreationAllowed
				onCheckedChanged: reticleModel.vertexCreationAllowed = checked
			}
		}

		Rectangle {
			height: 1
			Layout.fillWidth: true
			color: palette.mid
		}

		Item {
			opacity: reticleModel.canUndo ? 1.0 : 0.5
			Layout.fillWidth: true
			Layout.fillHeight: true

			Label {
				anchors.centerIn: parent
				text: qsTr("UNDO")
			}

			MouseArea {
				anchors.fill: parent
				onClicked: reticleModel.undoOrCancel()
				enabled: reticleModel.canUndo
			}
		}

		Rectangle {
			height: 1
			Layout.fillWidth: true
			color: palette.mid
		}

		Item {
			Layout.fillWidth: true
			Layout.fillHeight: true
			opacity: reticleModel.canRedo ? 1.0 : 0.5

			Label {
				anchors.centerIn: parent
				text: qsTr("REDO")
			}

			MouseArea {
				anchors.fill: parent
				enabled: reticleModel.canRedo
				onClicked: reticleModel.redo()
			}
		}

		Rectangle {
			height: 1
			Layout.fillWidth: true
			color: palette.mid
		}

		Item {
			Layout.fillWidth: true
			Layout.fillHeight: true
			opacity: reticleModel.selectedElementCanDelete ? 1.0 : 0.5
			Label {
				anchors.centerIn: parent
				text: qsTr("DELETE")
			}

			MouseArea {
				anchors.fill: parent
				enabled: reticleModel.selectedElementCanDelete
				onClicked: reticleModel.deleteSelectedElement()
			}
		}

		Rectangle {
			height: 1
			Layout.fillWidth: true
			color: palette.mid
		}

		Item {
			Layout.fillWidth: true
			Layout.fillHeight: true
			Label {
				anchors.centerIn: parent
				text: qsTr("CLOSE")
			}

			MouseArea {
				anchors.fill: parent
				onClicked: settingsPopup.visible = false
			}
		}

		// Spacer
		Item {
			height: 3
		}
	}
}
