// [WriteFile Name=UpdateAttributesFeatureService, Category=EditData]
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
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISRuntime.Toolkit.Controls 100.9

Rectangle {
    width: 800
    height: 600
    
    readonly property var featAttributes: ["Destroyed", "Major", "Minor", "Affected", "Inaccessible"]
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

            // set viewpoint over The United States
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

                    // make sure edits are successfully applied to the service
                    onApplyEditsStatusChanged: {
                        if (applyEditsStatus === Enums.TaskStatusCompleted) {
                            console.log("successfully updated feature");
                        }
                    }

                    // signal handler for the asynchronous updateFeature method
                    onUpdateFeatureStatusChanged: {
                        if (updateFeatureStatus === Enums.TaskStatusCompleted) {
                            // apply the edits to the service
                            featureTable.applyEdits();
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

                        // set the combo box's default value
                        damageComboBox.currentIndex = featAttributes.indexOf(damageType);
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
            updateWindow.visible = false;
        }

        onMouseClicked: {
            // reset the map callout and update window
            featureLayer.clearSelection();
            if (callout.visible)
                callout.dismiss();
            updateWindow.visible = false;
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
            // HTML to style the title text by centering it, increase pt size,
            // and bolding it.
            title: "<br><b><font size=\"+2\">%1</font></b>".arg(damageType)
            location: selectedFeature ? selectedFeature.geometry : null
        }

        Callout {
            id: callout
            calloutData: parent.calloutData;
            borderColor: "lightgrey"
            borderWidth: 1
            leaderPosition: leaderPositionEnum.Automatic
            onAccessoryButtonClicked: {
                updateWindow.visible = true;
            }
        }
    }

    // Update Window
    Rectangle {
        id: updateWindow
        width: childrenRect.width
        height: childrenRect.height
        anchors.centerIn: parent
        radius: 10
        visible: false

        GaussianBlur {
            anchors.fill: updateWindow
            source: mapView
            radius: 40
            samples: 20
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true;
            onWheel: wheel.accepted = true;
        }

        GridLayout {
            columns: 2
            anchors.margins: 5

            Text {
                Layout.columnSpan: 2
                Layout.margins: 5
                text: "Update Attribute"
                font.pixelSize: 16
            }

            ComboBox {
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width

                Layout.columnSpan: 2
                Layout.margins: 5
                Layout.fillWidth: true
                id: damageComboBox
                model: featAttributes

                Component.onCompleted : {
                    for (let i = 0; i < model.length; ++i) {
                        metrics.text = model[i];
                        modelWidth = Math.max(modelWidth, metrics.width);
                    }
                }
                TextMetrics {
                    id: metrics
                    font: damageComboBox.font
                }
            }

            Button {
                Layout.margins: 5
                text: "Update"

                function doUpdateAttribute(){
                    if (selectedFeature.loadStatus === Enums.LoadStatusLoaded) {
                        selectedFeature.onLoadStatusChanged.disconnect(doUpdateAttribute);

                        selectedFeature.attributes.replaceAttribute("typdamage", damageComboBox.currentText);
                        // update the feature in the feature table asynchronously
                        featureTable.updateFeature(selectedFeature);
                    }
                }

                // once the update button is clicked, hide the windows, and fetch the currently selected features
                onClicked: {
                    if (callout.visible)
                       callout.dismiss();
                    updateWindow.visible = false;

                    selectedFeature.onLoadStatusChanged.connect(doUpdateAttribute);
                    selectedFeature.load();
                }
            }

            Button {
                Layout.alignment: Qt.AlignRight
                Layout.margins: 5
                text: "Cancel"
                // once the cancel button is clicked, hide the window
                onClicked: updateWindow.visible = false;
            }
        }
    }
}
