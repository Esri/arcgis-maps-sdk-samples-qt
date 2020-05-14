// [WriteFile Name=ChangeViewpoint, Category=Maps]
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
import Esri.ArcGISRuntime 100.9

Rectangle {
    width: 800
    height: 600

    property real   rotationValue: 0.0
    property int    scaleIndex: -1

    PointBuilder {
        id: ptBuilder
        spatialReference: SpatialReference { wkid: 4326 }
    }

    EnvelopeBuilder {
        id: envBuilder
        spatialReference: SpatialReference { wkid: 4326 }
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

    MapView {
        id: mv
        anchors.fill: parent

        Map {
            BasemapImageryWithLabels {}
        }
    }

    ComboBox {
        id: comboBoxViewpoint
        anchors {
            left: parent.left
            top: parent.top
            margins: 5
        }

        property int modelWidth: 0
        width: modelWidth + leftPadding + rightPadding + indicator.width

        model: ["Center","Center and scale","Geometry","Geometry and padding","Rotation","Scale 1:5,000,000","Scale 1:10,000,000","Animation"]
        onCurrentTextChanged: {
            changeCurrentViewpoint(currentText);
        }

        Component.onCompleted: {
            for (let i = 0; i < model.length; ++i) {
                metrics.text = model[i];
                modelWidth = Math.max(modelWidth, metrics.width)
            }
        }

        TextMetrics {
            id: metrics
            font: comboBoxViewpoint.font
        }

        function changeCurrentViewpoint(text) {
            switch (text) {
            case "Center":
                ptBuilder.setXY(-117.195681, 34.056218); // Esri Headquarters
                mv.setViewpointCenter(ptBuilder.geometry);
                break;
            case "Center and scale":
                ptBuilder.setXY(-157.564, 20.677); // Hawai'i
                mv.setViewpointCenterAndScale(ptBuilder.geometry, 4000000.0);
                break;
            case "Geometry":
                envBuilder.setXY(116.380, 39.920, 116.400, 39.940); // Beijing
                mv.setViewpointGeometry(envBuilder.geometry);
                break;
            case "Geometry and padding":
                envBuilder.setXY(116.380, 39.920, 116.400, 39.940); // Beijing
                mv.setViewpointGeometryAndPadding(envBuilder.geometry, 200 * screenRatio());
                break;
            case "Rotation":
                rotationValue = (rotationValue + 45.0) % 360.0;
                mv.setViewpointRotation(rotationValue);
                break;
            case "Scale 1:5,000,000":
                mv.setViewpointScale(5000000.0);
                break;
            case "Scale 1:10,000,000":
                mv.setViewpointScale(10000000.0);
                break;
            case "Animation":
                mv.setViewpointWithAnimationCurve(springViewpoint, 4.0, Enums.AnimationCurveEaseInOutCubic);
                break;
            }
        }
    }

    function screenRatio() {
        const width = mv.mapWidth;
        const height = mv.mapHeight;
        return height > width ? width / height : height / width;
    }
}
