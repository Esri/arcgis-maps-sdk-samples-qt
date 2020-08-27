// [WriteFile Name=ListTransformations, Category=Geometry]
// [Legal]
// Copyright 2017 Esri.

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
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600
    
    property bool peDataSet: true
    readonly property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/PEDataRuntime"

    Component.onCompleted: TransformationCatalog.projectionEngineDirectory = dataPath

    Connections {
        target: TransformationCatalog
        function onErrorChanged() {
            peDataSet = false;
        }
    }

    MapView {
        id: mapView
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: parent.height / 2

        Map {
            id: map
            BasemapLightGrayCanvas {}

            ViewpointCenter {
                center: originalGeometry
                targetScale: 5000
            }

            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;

                getTransformations();
                statusBar.expand();
            }
        }

        // Create a GraphicsOverlay with two Graphics - one to hold the default
        // transformed geometry, and the other to use the selected transformation
        GraphicsOverlay {
            Graphic {
                geometry: originalGeometry

                SimpleMarkerSymbol {
                    style: Enums.SimpleMarkerSymbolStyleSquare
                    color: "blue"
                    size: 20
                }
            }

            Graphic {
                id: projectedGraphic
                geometry: originalGeometry

                SimpleMarkerSymbol {
                    style: Enums.SimpleMarkerSymbolStyleCross
                    color: "red"
                    size: 20
                }
            }
        }

        // Create a geometry located in the Greenwich observatory courtyard in London, UK, the location of the
        // Greenwich prime meridian. This will be projected using the selected transformation.
        Point {
            id: originalGeometry
            x: 538985.355
            y: 177329.516
            SpatialReference { wkid: 27700 }
        }
    }

    Rectangle {
        id: transformationView

        anchors {
            left: parent.left
            right: parent.right
            top: mapView.bottom
        }

        height: parent.height / 2
        color: "#f7f7f7"

        CheckBox {
            id: orderCheckbox
            anchors {
                left: parent.left
                top: parent.top
                margins: 10
            }
            text: "Order by suitability for map extent"
            onCheckedChanged: {
                getTransformations();
            }
        }

        ListView {
            id: transformationList
            anchors {
                left: parent.left
                right: parent.right
                top: orderCheckbox.bottom
                bottom: parent.bottom
                margins: 10
            }
            clip: true

            delegate: Item {
                id: itemDelegate
                height: 45
                width: parent.width
                clip: true

                // show the DatumTransformation name
                Text {
                    id: label
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        right: parent.right
                    }
                    width: parent.width
                    text: model.modelData.missingProjectionEngineFiles ? "%1 <font color=red><b>Missing grid files</b></font>".arg(model.modelData.name) : model.modelData.name
                    textFormat: Text.RichText
                    wrapMode: Text.WrapAnywhere
                    maximumLineCount: 2
                    font.pixelSize: 12
                }

                MouseArea {
                    id: itemMouseArea
                    anchors.fill: parent
                    onClicked: {
                        transformationList.currentIndex = index;
                        const transform = transformationList.model[index];
                        if (transform.missingProjectionEngineFiles) {
                            let missingFiles = "Missing grid files: ";
                            const steps = transform.steps;
                            for (let i = 0; i < steps.length; i++) {
                                for (let j = 0; j < steps[i].projectionEngineFilenames.length; j++) {
                                    missingFiles += steps[i].projectionEngineFilenames[j];
                                }
                            }
                            statusText.text = missingFiles + " ";
                            if (statusBar.isExpanded)
                                timer.restart();
                            else
                                statusBar.expand();
                        } else {
                            projectedGraphic.geometry = GeometryEngine.projectWithDatumTransformation(originalGeometry, map.spatialReference, transform);
                        }
                    }
                }
            }

            highlightMoveDuration: 1
            highlightFollowsCurrentItem: true
            highlight: Rectangle {
                color: "#d6d6d6"
                radius: 4
            }
        }
    }

    Rectangle {
        id: statusBar
        property int expanded: parent.height - height
        property int hidden: parent.height
        property bool isExpanded: y === expanded
        anchors {
            left: parent.left
            right: parent.right
        }
        height: 45
        color: "black"
        y: hidden

        Text {
            id: statusText
            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
                margins: 10
            }
            color: "white"
            wrapMode: Text.WrapAnywhere
            text: peDataSet ?
                      "Projection engine directory set %1".arg(dataPath) :
                      "Error setting projection engine directory: %1. %2".arg(TransformationCatalog.error.message).arg(TransformationCatalog.error.additionalMessage)
        }

        Timer {
            id: timer
            interval: 5000
            onTriggered: statusBar.animate();
        }

        onYChanged: {
            if (y === expanded)
                timer.restart();
        }

        NumberAnimation {
            id: statusAnimation
            target: statusBar
            properties: "y"
            duration: 500
            easing.type: Easing.OutQuad
        }

        function expand() {
            statusAnimation.from = statusBar.hidden;
            statusAnimation.to = statusBar.expanded;
            statusAnimation.start();
        }

        function hide() {
            statusAnimation.from = statusBar.expanded;
            statusAnimation.to = statusBar.hidden;
            statusAnimation.start();
        }

        function animate() {
            if (statusBar.y === statusBar.hidden)
                expand();
            else
                hide();
        }
    }

    function getTransformations() {
        if (orderCheckbox.checked)
            transformationList.model = TransformationCatalog.transformationsBySuitabilityWithAreaOfInterest(originalGeometry.spatialReference, map.spatialReference, mapView.visibleArea.extent);
        else
            transformationList.model = TransformationCatalog.transformationsBySuitability(originalGeometry.spatialReference, map.spatialReference);
    }
}
