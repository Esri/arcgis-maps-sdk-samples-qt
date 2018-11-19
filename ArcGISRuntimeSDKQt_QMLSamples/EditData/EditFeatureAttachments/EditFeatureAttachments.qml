// [WriteFile Name=EditFeatureAttachments, Category=EditData]
// [Legal]
// Copyright 2016 Esri.

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
import QtQuick.Controls 2.2
import Qt.labs.platform 1.0
import QtGraphicalEffects 1.0
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime 100.5
import Esri.ArcGISRuntime.Toolkit.Controls 100.5

Rectangle {

    property string damageType
    property var selectedFeature: null

    // Create MapView that contains a Map
    MapView {
        id: mapView
        anchors.fill: parent
        wrapAroundMode: Enums.WrapAroundModeDisabled

        Map {
            // Set the initial basemap to Streets
            BasemapStreets { }

            ViewpointCenter {
                Point {
                    x: -10800000
                    y: 4500000
                    spatialReference: SpatialReference {
                        wkid: 102100
                    }
                }
                targetScale: 3e7
            }

            FeatureLayer {
                id: featureLayer

                // declare as child of feature layer, as featureTable is the default property
                ServiceFeatureTable {
                    id: featureTable
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"

                    onApplyEditsStatusChanged: {
                        if (applyEditsStatus === Enums.TaskStatusCompleted) {
                            console.log("successfully applied attachment edits to service");

                            // update the selected feature with attributes
                            featureLayer.selectFeaturesWithQuery(params, Enums.SelectionModeNew);
                        }
                    }
                }

                // signal handler for selecting features
                onSelectFeaturesStatusChanged: {
                    if (selectFeaturesStatus === Enums.TaskStatusCompleted) {
                        if (!selectFeaturesResult.iterator.hasNext)
                            return;

                        selectedFeature = selectFeaturesResult.iterator.next();
                        damageType = selectedFeature.attributes.attributeValue("typdamage");

                        // show the callout
                        callout.showCallout();
                    }
                }
            }
        }

        QueryParameters {
            id: params
        }

        // hide the callout after navigation
        onViewpointChanged: {
            if (callout.visible)
                callout.dismiss();
            attachmentWindow.visible = false;
        }

        onMouseClicked: {
            // reset to defaults
            featureLayer.clearSelection();
            if (callout.visible)
                callout.dismiss();
            attachmentWindow.visible = false;
            selectedFeature = null;

            // call identify on the mapview
            mapView.identifyLayerWithMaxResults(featureLayer, mouse.x, mouse.y, 10, false, 1);
        }

        onIdentifyLayerStatusChanged: {
            if (identifyLayerStatus === Enums.TaskStatusCompleted) {
                if (identifyLayerResult.geoElements.length > 0) {
                    // get the objectid of the identifed object
                    params.objectIdsAsInts = [identifyLayerResult.geoElements[0].attributes.attributeValue("objectid")];
                    // query for the feature using the objectid
                    featureLayer.selectFeaturesWithQuery(params, Enums.SelectionModeNew);
                }
            }
        }

        calloutData {
            title: "<b>%1</b>".arg(damageType)
            location: selectedFeature ? selectedFeature.geometry : null;
            detail: selectedFeature === null ? "" : "Number of attachments: %1".arg(selectedFeature.attachments.count)
        }

        Callout {
            id: callout
            borderColor: "lightgrey"
            borderWidth: 1
            calloutData : parent.calloutData
            leaderPosition: leaderPositionEnum.Automatic
            onAccessoryButtonClicked: {
                attachmentWindow.visible = true;
            }

        }
    }

    // attachment window
    Rectangle {
        id: attachmentWindow
        anchors.centerIn: parent
        height: 200
        width: 250
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
            height: 40
            color: "transparent"

            Text {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    margins: 10
                }

                text: "Attachments"; font {bold: true; pixelSize: 20;}
            }

            Row {
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: 10
                }
                spacing: 15
                Text {
                    text: "+"; font {bold: true; pixelSize: 40;} color: "green"

                    // open a file dialog whenever the add button is clicked
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            fileDialog.open();
                        }
                    }
                }
                Text {
                    text: "-"; font {bold: true; pixelSize: 40;} color: "red"

                    // make sure an item is selected and if so, delete it from the service
                    MouseArea {
                        anchors.fill: parent

                        function doDeleteAttachment(){
                            if (selectedFeature.loadStatus === Enums.LoadStatusLoaded) {
                                selectedFeature.onLoadStatusChanged.disconnect(doDeleteAttachment);
                                selectedFeature.attachments.deleteAttachmentWithIndex(attachmentsList.currentIndex);
                            }
                        }

                        onClicked: {
                            if (attachmentsList.currentIndex !== -1)  {
                                // delete the attachment from the table
                                if (selectedFeature.loadStatus === Enums.LoadStatusLoaded) {
                                    selectedFeature.attachments.deleteAttachmentWithIndex(attachmentsList.currentIndex);
                                } else {
                                    selectedFeature.onLoadStatusChanged.connect(doDeleteAttachment);
                                    selectedFeature.load();
                                }
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
                margins: 10
            }
            clip: true
            spacing: 5
            // set the model equal to the currently selected feature's attachment list model
            model: selectedFeature === null ? null : selectedFeature.attachments
            // create the delegate to specify how the view is arranged
            delegate: Item {
                height: 45
                width: parent.width
                clip: true

                // show the attachment name
                Text {
                    id: label
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        right: attachment.left
                    }
                    text: name
                    wrapMode: Text.WrapAnywhere
                    maximumLineCount: 1
                    elide: Text.ElideRight
                    font.pixelSize: 16
                }

                // show the attachment's URL if it is an image
                Image {
                    id: attachment
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                    }
                    width: 44
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
                height: attachmentsList.currentItem != null ? attachmentsList.currentItem.height
                                                            : 0
                color: "lightsteelblue"
            }
        }
    }

    // file dialog for selecting a file to add as an attachment
    //! [EditFeatures add attachment from a file dialog]
    FileDialog {
        id: fileDialog
        folder: {
            var locs = StandardPaths.standardLocations(StandardPaths.PicturesLocation)
            return locs.length > 0 ? locs.last() : "";
        }

        function doAddAttachment(){
            if (selectedFeature.loadStatus === Enums.LoadStatusLoaded) {
                selectedFeature.onLoadStatusChanged.disconnect(doAddAttachment);
                selectedFeature.attachments.addAttachment(fileDialog.file, "application/octet-stream", fileInfo.fileName);
            }
        }

        onAccepted: {
            // add the attachment to the feature table
            fileInfo.url = fileDialog.file;
            if (selectedFeature.loadStatus === Enums.LoadStatusLoaded) {
                selectedFeature.attachments.addAttachment(fileDialog.file, "application/octet-stream", fileInfo.fileName);
            } else {
                selectedFeature.onLoadStatusChanged.connect(doAddAttachment);
                selectedFeature.load();
            }
        }
    }
    //! [EditFeatures add attachment from a file dialog]

    // file info used for obtaining the file name
    FileInfo {
        id: fileInfo
    }
}
