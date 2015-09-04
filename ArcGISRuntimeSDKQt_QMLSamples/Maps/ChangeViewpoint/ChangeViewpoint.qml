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
import QtQuick.Controls.Styles 1.2
import Esri.ArcGISRuntime 100.00
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real   scaleFactor: System.displayScaleFactor
    property real   rotationValue: 0.0
    property int    scaleIndex: -1

    PointBuilder {
        id: ptBuilder
        spatialReference: SpatialReference.createWgs84()
    }

    EnvelopeBuilder {
        id: envBuilder
        spatialReference: SpatialReference.createWgs84()
    }

    ViewpointExtent {
        id: springViewpoint
        extent: Envelope {
            xMin: -12338668.348591767
            xMax: -12338247.594362013
            yMin: 5546908.424239618
            yMax: 5547223.989911933
            spatialReference: SpatialReference { wkid: 102100 }
        }
    }

    // Map view UI presentation at top
    MapView {
        id: mv
        anchors.fill: parent
        Map {
            BasemapImageryWithLabels {}
        }
    }

//    Rectangle {
//        anchors {
//            bottom: parent.bottom
//            left: parent.left
//            margins: 5 * scaleFactor
//        }
//        color: "white"
//        opacity: 1.0
//        height: flow1.height * scaleFactor
//     //   implicitWidth: flow1.width * scaleFactor
//        width: fattestButton.width + (5 * scaleFactor)

//        Component.onCompleted: {
//            console.log("height:", height);
//            console.log("width:", width);
//        }

        // Controls UI presentation at bottom
        Flow {
            id: flow1
            anchors {
                bottom: parent.bottom
                left: parent.left
                margins: 5 * scaleFactor
            }
            width: parent.width
            spacing: 5

            Button {
                text: "Center"
                onClicked: {
                    ptBuilder.setXYZM(-117.195681, 34.056218, 0.0, 0.0); // Esri Headquarters
                    mv.setViewpointCenter(ptBuilder.geometry);
                }
            }
            Button {
                text: "Center & Scale"
                onClicked: {
                    ptBuilder.setXYZM(-157.564, 20.677, 0.0, 0.0); // Hawai'i
                    mv.setViewpointCenterAndScale(ptBuilder.geometry, 4000000.0);
                }
            }
            Button {
                text: "Geometry"
                onClicked: {
                    envBuilder.setCoords(116.385, 39.92, 116.395, 39.93, 0, 0, 0, 0); // Beijing
                    mv.setViewpointGeometry(envBuilder.geometry);
                }
            }
            Button {
                id: fattestButton
                text: "Geometry && Padding"
                onClicked: {
                    envBuilder.setCoords(116.385, 39.92, 116.395, 39.93, 0, 0, 0, 0); // Beijing
                    mv.setViewpointGeometryAndPadding(envBuilder.geometry, 200);
                }
            }
            Button {
                text: "Rotation"
                onClicked: {
                    rotationValue = (rotationValue + 45.0) % 360.0;
                    mv.setViewpointRotation(rotationValue);
                }
            }
            Button {
                text: "Scale"
                onClicked: {
                    var scaleValues = [2000000.0, 5000000.0, 10000000.0, 50000000.0];
                    var scaleCount = scaleValues.length;

                    scaleIndex += 1;
                    scaleIndex = scaleIndex % scaleValues.length;
                    console.log("scaleIndex: ", scaleIndex);
                    console.log("scaleValues.length: ", scaleValues.length);
                    mv.setViewpointScale(scaleValues[scaleIndex]);
                }
            }
            Button {
                text: "Animation"
                onClicked: {
                    mv.setViewpointWithAnimationCurve(springViewpoint, 4.0, Enums.AnimationCurveEaseInOutCubic);
                }
            }
//        }
  }

    // Neatline rectangle
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
