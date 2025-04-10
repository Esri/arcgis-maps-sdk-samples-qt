import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.Samples

Item {
    // MapView component
    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            forceActiveFocus(); // Enable keyboard navigation
        }
    }

    // Declare the C++ instance and bind the mapView
    SnapGeometryEditsWithRulesSample {
        id: snapGeometryEditsWithRulesModel
        mapView: mapView
    }

    Rectangle {
        id: instructionsPanel
        visible: snapGeometryEditsWithRulesModel.isElementSelected ? false : true
        anchors {
            right: parent.right
            margins: 10
        }
        width: textItem.width + 20
        height: textItem.height + 10
        color: "black"
        opacity: .5

        Text {
            id: textItem
            anchors.centerIn: parent
            text: "Tap a point feature to edit."
            color: "white"
            font.pixelSize: 16
            font.bold: true
        }
    }


    Control {
        id: control
        visible: snapGeometryEditsWithRulesModel.isElementSelected ? true : false
        anchors.right: parent.right
        padding: 5

        background: Rectangle {
            color: "black"
            opacity: .5
        }
        contentItem: ColumnLayout {
            id: columns
            anchors {
                verticalCenter: parent.verticalCenter
                horizontalCenter: parent.horizontalCenter
            }

            GridLayout {
                id: geometryColumn
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                columns: 2

                Text {
                    id: geometryHeader
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.columnSpan: 2
                    text: "Feature selected"
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                Text {
                    id: assetGroupText
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.columnSpan: 2
                    text: "AssetGroup: "
                    color: "white"
                    font.pixelSize: 12
                }

                Text {
                    id: assetTypeText
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.columnSpan: 2
                    text: "AssetType: "
                    color: "white"
                    font.pixelSize: 12
                }

                Connections {
                    target: snapGeometryEditsWithRulesModel
                    function onAssetGroupChanged(assetGroup) {
                        assetGroupText.text = "AssetGroup: " + assetGroup;
                    }
                    function onAssetTypeChanged(assetType) {
                        assetTypeText.text = "AssetType: " + assetType;
                    }
                }
            }

            GridLayout {
                id: editingColumn
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                columns: 2

                GeometryEditorButton {
                    id: startButton
                    buttonName: qsTr("Start editor")
                    iconPath: ""
                    Layout.columnSpan: 2
                    enabled: !snapGeometryEditsWithRulesModel.geometryEditorStarted
                    onClicked: snapGeometryEditsWithRulesModel.startEditor();
                }
                GeometryEditorButton {
                    id: snapSettingsButton
                    buttonName: qsTr("Snap Settings")
                    iconPath: "qrc:/Samples/EditData/SnapGeometryEditsWithRules/iconAssets/settings.png"
                    Layout.columnSpan: 2
                    enabled: snapGeometryEditsWithRulesModel.isElementSelected
                    onClicked: {
                        optionPanel.visible = !optionPanel.visible
                    }
                }
                GeometryEditorButton {
                    id: saveButton
                    buttonName: qsTr("Save")
                    iconPath: "qrc:/Samples/EditData/SnapGeometryEditsWithRules/iconAssets/save-32.png"
                    enabled: snapGeometryEditsWithRulesModel.geometryEditorStarted
                    onClicked: snapGeometryEditsWithRulesModel.stopEditing();
                }

                GeometryEditorButton {
                    id: discardButton
                    buttonName: qsTr("Discard")
                    iconPath: "qrc:/Samples/EditData/SnapGeometryEditsWithRules/iconAssets/trash-32.png"
                    enabled: snapGeometryEditsWithRulesModel.geometryEditorStarted
                    onClicked: snapGeometryEditsWithRulesModel.discardEdits();
                }
            }

            GridLayout {
                id: snappingLegend
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                columns: 2

                Rectangle {
                    width: 10
                    height: 10
                    color: "green"
                    radius: 2
                }
                Text {
                    text: "None"
                    color: "white"
                    font.pixelSize: 12
                }

                Rectangle {
                    width: 10
                    height: 10
                    color: "orange"
                    radius: 2
                }
                Text {
                    text: "RulesLimitSnapping"
                    color: "white"
                    font.pixelSize: 12
                }

                Rectangle {
                    width: 10
                    height: 10
                    color: "red"
                    radius: 2
                }
                Text {
                    text: "RulesPreventSnapping"
                    color: "white"
                    font.pixelSize: 12
                }
            }
        }
    }

    Rectangle {
        id: optionPanel
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        width: 220
        visible: false
        color: "black"
        opacity: .5

        ListView {
            id: snapSourceView
            anchors {
                fill: parent
                margins: 10
            }

            header: ColumnLayout {
                id: snappingColumn
                Layout.minimumWidth: optionPanel.width
                spacing: 0
                RowLayout {
                    Layout.minimumWidth: optionPanel.width
                    Layout.minimumHeight: 35

                    Text {
                        Layout.alignment: Qt.AlignLeft
                        Layout.fillWidth: true
                        Layout.minimumWidth: optionPanel.width * 0.75
                        text: "Snapping"
                        font.pixelSize: 12
                        color: "white"
                        font.bold: true
                    }

                    Text {
                        Layout.alignment: Qt.AlignRight
                        Layout.fillWidth: true
                        Layout.minimumWidth: optionPanel.width * 0.25
                        text: "Done"
                        font.pixelSize: 12
                        color: "white"
                        font.bold: true
                        MouseArea {
                            anchors.fill: parent
                            onClicked: optionPanel.visible = false;
                        }
                    }
                }
            }

            model: snapGeometryEditsWithRulesModel.snapSourceListModel

            delegate: Item {
                height: 35
                width: optionPanel.width
                id:delegate
                Rectangle {
                    id: wrapper
                    color: "#E9DFEA"
                    width: snapSourceView.width - (snapSourceView.anchors.margins / 2)
                    height: delegate.height
                    anchors {
                        margins: 15
                    }

                    RowLayout {
                        id : row
                        Layout.fillWidth: true
                        Layout.minimumWidth: optionPanel.width
                        width: wrapper.width

                        Text {
                            Layout.alignment: Qt.AlignLeft
                            Layout.fillWidth: true
                            Layout.minimumWidth: optionPanel.width / 2
                            text: name
                            font.pixelSize: 12
                            Layout.leftMargin: 10
                        }

                        Switch {
                            Layout.alignment: Qt.AlignRight
                            Layout.fillWidth: true
                            checked: isEnabled
                            onCheckedChanged: {
                                if (isEnabled !== checked) {
                                    isEnabled = checked; // Update only if there's a difference
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
