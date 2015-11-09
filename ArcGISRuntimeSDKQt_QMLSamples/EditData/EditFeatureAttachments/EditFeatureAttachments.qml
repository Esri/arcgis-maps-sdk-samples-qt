// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import Esri.ArcGISRuntime 100.00
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property double mousePointX
    property double mousePointY
    property string damageType
    property var selectedFeature: null

    // Create MapView that contains a Map
    MapView {
        id: mapView
        anchors.fill: parent
        wrapAroundEnabled: false

        Map {
            // Set the initial basemap to Streets
            BasemapStreets { }

            // Set the initial viewpoint over the UK
            initialViewpoint: ViewpointCenter {
                center: Point {
                    x: 14902768.01
                    y: 4050890.22
                    spatialReference: SpatialReference { wkid: 102100 }
                }
                scale: 2e6
            }

            FeatureLayer {
                id: featureLayer

                selectionColor: "cyan"
                selectionWidth: 3 * scaleFactor

                // declare as child of feature layer, as featureTable is the default property
                ServiceFeatureTable {
                    id: featureTable
                    url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"

                    onApplyEditsStatusChanged: {
                        if (applyEditsStatus === Enums.TaskStatusCompleted) {
                            console.log("successfully added attachment to service");
                        }
                    }
                }

                // signal handler for selecting features
                onSelectFeaturesStatusChanged: {
                    if (selectFeaturesStatus === Enums.TaskStatusCompleted) {
                        if (!selectFeaturesResult.iterator.hasNext)
                            return;

                        selectedFeature  = selectFeaturesResult.iterator.next();
                        damageType = selectedFeature.attributes["typdamage"];
                        // fetch the attachment infos from the service
                        selectedFeature.attachmentListModel.fetchAttachmentInfos();

                        // show the callout
                        callout.x = mousePointX;
                        callout.y = mousePointY;
                        callout.visible = true;
                    }
                }
            }
        }

        QueryParameters {
            id: params
            outFields: ["*"]
            maxFeatures: 1
        }

        // hide the callout after navigation
        onVisibleAreaChanged: {
            callout.visible = false;
            attachmentWindow.visible = false;
        }

        onMouseClicked: {
            console.log(JSON.stringify(mouse.mapPoint.json))
            // reset to defaults
            featureLayer.clearSelection();
            callout.visible = false;
            attachmentWindow.visible = false;
            selectedFeature = null;
            mousePointX = mouse.x;
            mousePointY = mouse.y - callout.height;

            // call identify on the mapview
            mapView.identifyLayer(featureLayer, mouse.x, mouse.y, 10 * scaleFactor, 1);
        }

        onIdentifyLayerStatusChanged: {
            if (identifyLayerStatus === Enums.TaskStatusCompleted) {
                if (identifyLayerResults.length > 0) {
                    // get the objectid of the identifed object
                    params.objectIds = [identifyLayerResults[0].attributes["objectid"]];
                    // query for the feature using the objectid
                    featureLayer.selectFeaturesWithQuery(params, Enums.SelectionModeNew);
                }
            }
        }
    }

    // map callout window
    Rectangle {
        id: callout
        width: col.width + (10 * scaleFactor) // add 10 for padding
        height: 60 * scaleFactor
        radius: 5
        border {
            color: "lightgrey"
            width: .5
        }
        visible: false

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
        }

        Column {
            id: col
            anchors {
                top: parent.top
                left: parent.left
                margins: 5 * scaleFactor
            }
            spacing: 10

            Row {
                spacing: 10

                Text {
                    text: damageType
                    font.pixelSize: 18 * scaleFactor
                }

                Rectangle {
                    radius: 100
                    width: 22 * scaleFactor
                    height: width
                    color: "transparent"
                    border.color: "blue"
                    antialiasing: true

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "i"
                        font.pixelSize: 18 * scaleFactor
                        color: "blue"
                    }

                    // create a mouse area over the (i) text to open the update window
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            attachmentWindow.visible = true;
                        }
                    }
                }
            }

            Row {
                spacing: 10

                Text {
                    id: attachmentText
                    text: selectedFeature === null ? "" : "Number of attachments: %1".arg(selectedFeature.attachmentListModel.count)
                    font.pixelSize: 12 * scaleFactor
                }
            }
        }

    }

    // attachment window
    Rectangle {
        id: attachmentWindow
        anchors.centerIn: parent
        height: 200 * scaleFactor
        width: 250 * scaleFactor
        visible: false
        radius: 10
        color: "lightgrey"
        border.color: "darkgrey"
        opacity: 0.90
        clip: true

        // accept mouse events so they do not propogate down to the map
        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        Rectangle {
            id: titleText
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: 40 * scaleFactor
            color: "transparent"

            Text {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    margins: 10 * scaleFactor
                }

                text: "Attachments"; font {bold: true; pixelSize: 20 * scaleFactor;}
            }

            Row {
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: 10 * scaleFactor
                }
                spacing: 15
                Text {
                    text: "+"; font {bold: true; pixelSize: 40 * scaleFactor;} color: "green"

                    // open a file dialog whenever the add button is clicked
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            fileDialog.open();
                        }
                    }
                }
                Text {
                    text: "-"; font {bold: true; pixelSize: 40 * scaleFactor;} color: "red"

                    // make sure an item is selected and if so, delete it from the service
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (attachmentsList.currentIndex === -1)  {
                                msgDialog.text = "Please first select an attachment to delete.";
                                msgDialog.open();
                            } else {
                                // delete the attachment from the table
                                selectedFeature.attachmentListModel.deleteAttachmentWithIndex(attachmentsList.currentIndex);

                                // once the status is complete, call apply edits to apply edits from the feature table to the service
                                selectedFeature.attachmentListModel.deleteAttachmentStatusChanged.connect(function () {
                                    if (selectedFeature.attachmentListModel.deleteAttachmentStatus === Enums.TaskStatusCompleted) {
                                        featureTable.applyEdits();
                                    }
                                });
                            }
                        }
                    }
                }
            }
        }

        ListView {
            id: attachmentsList
            anchors {
                left: parent.left
                right: parent.right
                top: titleText.bottom
                bottom: parent.bottom
                margins: 10 * scaleFactor
            }
            clip: true
            spacing: 5
            // set the model equal to the currently selected feature's attachment list model
            model: selectedFeature === null ? null : selectedFeature.attachmentListModel
            // create the delegate to specify how the view is arranged
            delegate: Item {
                height: 45* scaleFactor
                width: parent.width
                clip: true

                // show the attachment name
                Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                    }
                    text: name
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    font.pixelSize: 16 * scaleFactor
                }

                // show the attachment's URL if it is an image
                Image {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                    }
                    width: 44 * scaleFactor
                    height: width
                    fillMode: Image.PreserveAspectFit
                    source: attachmentUrl
                }

                MouseArea {
                    id: itemMouseArea
                    anchors.fill: parent
                    onClicked: {
                        attachmentsList.currentIndex = index;
                    }
                }
            }

            highlightFollowsCurrentItem: true
            highlight: Rectangle {
                height: attachmentsList.currentItem.height
                color: "lightsteelblue"
            }
        }
    }

    // file dialog for selecting a file to add as an attachment
    FileDialog {
        id: fileDialog
        onAccepted: {
            // add the attachment to the feature table
            fileInfo.url = fileDialog.fileUrl;
            selectedFeature.attachmentListModel.addAttachment(fileDialog.fileUrl, "application/octet-stream", fileInfo.fileName);

            // connect to the status changed signal
            // once the status is complete, call apply edits to apply edits from the feature table to the service
            selectedFeature.attachmentListModel.addAttachmentStatusChanged.connect(function () {
                if (selectedFeature.attachmentListModel.addAttachmentStatus === Enums.TaskStatusCompleted) {
                    featureTable.applyEdits();
                }
            });
        }
    }

    MessageDialog {
        id: msgDialog
    }

    // file info used for obtaining the file name
    FileInfo {
        id: fileInfo
    }

    // neatline
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
