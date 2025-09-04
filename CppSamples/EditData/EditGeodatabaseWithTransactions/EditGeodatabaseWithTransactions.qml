// [WriteFile Name=EditGeodatabaseWithTransactions, Category=EditData]
// [Legal]
// Copyright 2025 Esri.

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

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Status text overlay
    Rectangle {
        id: statusOverlay
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: statusText.implicitHeight + (parent.height * 0.02)
        opacity: 0.9

        Text {
            id: statusText
            anchors.centerIn: parent
            text: gdbModel.messageText
            font.pointSize: Math.max(10, parent.height * 0.025)
            wrapMode: Text.WordWrap
            width: parent.width - (parent.width * 0.05)
            horizontalAlignment: Text.AlignHCenter
        }
    }

    // Loading progress bar
    ProgressBar {
        id: loadingProgressBar
        anchors {
            bottom: bottomToolbar.top
            left: parent.left
            right: parent.right
            leftMargin: parent.width * 0.05
            rightMargin: parent.width * 0.05
            bottomMargin: parent.height * 0.01
        }
        visible: gdbModel.loadingVisible
        indeterminate: true
    }

    // Bottom toolbar
    Rectangle {
        id: bottomToolbar
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: Math.max(parent.height * 0.08, 50)
        border.color: "gray"
        border.width: 1

        RowLayout {
            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
                margins: 10
            }
            spacing: 20

            // Start/Stop Transaction button
            Button {
                id: transactionButton
                text: gdbModel.stopEditingEnabled ? "End Transaction" : "Start Transaction"
                enabled: gdbModel.requireTransaction
                onClicked: {
                    if (gdbModel.stopEditingEnabled) {
                        gdbModel.stopTransaction()
                    } else {
                        gdbModel.beginTransaction()
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            // Require Transaction toggle
            CheckBox {
                id: requireTransactionCheckBox
                text: "Requires Transaction"
                checked: true
                enabled: !gdbModel.stopEditingEnabled
                onCheckedChanged: {
                    gdbModel.requireTransaction = checked
                }
            }
        }
    }

    // Feature type selection dialog
    Dialog {
        id: featureTypeDialog
        title: "New Feature"
        anchors.centerIn: parent
        width: Math.min(500, parent.width * 0.9)
        height: Math.min(600, parent.height * 0.8)

        property point tapLocation: Qt.point(0, 0)

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 12

            Label {
                text: "Feature Table"
                font.bold: true
            }

            // Dynamic feature table selection
            ListView {
                id: tableSelectionList
                Layout.fillWidth: true
                Layout.preferredHeight: Math.min(contentHeight, 96)
                clip: true
                boundsBehavior: Flickable.StopAtBounds

                model: gdbModel.availableTableNames

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }

                delegate: ItemDelegate {
                    width: tableSelectionList.width
                    implicitHeight: 32

                    Rectangle {
                        anchors.fill: parent
                        color: gdbModel.selectedTableName === modelData ? "#7938b6" : "transparent"
                        border.color: "gray"
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            text: modelData
                            color: gdbModel.selectedTableName === modelData ? "white" : "black"
                        }
                    }

                    onClicked: {
                        gdbModel.selectedTableName = modelData
                    }
                }

                Component.onCompleted: {
                    if (gdbModel.selectedTableName && gdbModel) {
                        for (let i = 0; i < model.length; i++) {
                            if (model[i] === gdbModel.selectedTableName) {
                                currentIndex = i
                                break
                            }
                        }
                    }
                }

                Connections {
                    target: gdbModel
                    function onSelectedTableNameChanged() {
                        if (gdbModel.selectedTableName && tableSelectionList.model) {
                            for (let i = 0; i < tableSelectionList.model.length; i++) {
                                if (tableSelectionList.model[i] === gdbModel.selectedTableName) {
                                    tableSelectionList.currentIndex = i
                                    break
                                }
                            }
                        }
                    }
                }
            }

            Label {
                text: "Feature Type"
                font.bold: true
            }

            // Feature type list - populated from geodatabase
            ListView {
                id: featureTypeList
                Layout.fillWidth: true

                Layout.fillHeight: true
                Layout.preferredHeight: 128

                clip: true
                boundsBehavior: Flickable.StopAtBounds

                property string selectedFeatureType: ""

                model: gdbModel.currentFeatureTypes

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }

                delegate: ItemDelegate {
                    width: featureTypeList.width
                    implicitHeight: 32

                    Rectangle {
                        anchors.fill: parent
                        color: featureTypeList.currentIndex === index ? "#7938b6" : "transparent"
                        border.color: "gray"
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            text: modelData
                            color: featureTypeList.currentIndex === index ? "white" : "black"
                        }
                    }

                    onClicked: {
                        featureTypeList.selectedFeatureType = modelData
                        featureTypeList.currentIndex = index
                    }
                }

                onModelChanged: {
                    if (model && model.length > 0) {
                        currentIndex = 0
                        selectedFeatureType = model[0]
                    }
                }

                Component.onCompleted: {
                    if (model && model.length > 0) {
                        currentIndex = 0
                        selectedFeatureType = model[0]
                    }
                }
            }
        }

        footer: DialogButtonBox {
            Button {
                text: "Done"
                enabled: featureTypeList.selectedFeatureType !== ""
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                onClicked: {
                    var tableName = gdbModel.selectedTableName
                    gdbModel.addFeatureAtLocation(tableName, featureTypeList.selectedFeatureType, featureTypeDialog.tapLocation)
                    featureTypeDialog.accept()
                }
            }
            Button {
                text: "Cancel"
                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                onClicked: featureTypeDialog.reject()
            }
        }
    }

    Dialog {
        id: commitDialog
        title: "Commit Edits"
        anchors.centerIn: parent
        width: Math.min(400, parent.width * 0.9)
        modal: true

        contentItem: Column {
            spacing: 10

            Label {
                text: "Commit the edits in the transaction to the geodatabase or rollback to discard them."
                width: parent.width - 40
                wrapMode: Text.WordWrap
            }
        }

        footer: DialogButtonBox {
            Button {
                text: "Commit"
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                onClicked: {
                    commitDialog.close()
                    gdbModel.commitTransaction()
                }
            }

            Button {
                text: "Rollback"
                DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
                onClicked: {
                    commitDialog.close()
                    gdbModel.rollbackTransaction()
                }
            }

            Button {
                text: "Cancel"
                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                onClicked: {
                    commitDialog.close()
                    gdbModel.cancelTransaction()
                }
            }
        }
    }



    // Declare the C++ instance which creates the map etc. and supply the view
    EditGeodatabaseWithTransactionsSample {
        id: gdbModel
        mapView: view

        onCommitDialogRequested: {
            commitDialog.open()
        }

        onFeatureTypeSelectionRequested: function(x, y)
        {
            featureTypeDialog.tapLocation = Qt.point(x, y)
            if (gdbModel.selectedTableName)
            {
                gdbModel.updateFeatureTypesForSelectedTable(gdbModel.selectedTableName)
            }

            Qt.callLater(function() {
                if (featureTypeList.model && featureTypeList.model.length > 0) {
                    featureTypeList.currentIndex = 0
                    featureTypeList.selectedFeatureType = featureTypeList.model[0]
                }
            })

            featureTypeDialog.open()
        }
    }
}
