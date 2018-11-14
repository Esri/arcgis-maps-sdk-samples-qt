// [WriteFile Name=DensifyAndGeneralize, Category=Geometry]
// [Legal]
// Copyright 2018 Esri.

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
import Esri.ArcGISRuntime 100.5

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600    

    MapView {
        id: mapView
        anchors.fill: parent

        // Declare a map with the streets night vector basemap
        Map {
            BasemapStreetsNightVector {}
        }

        // Add a graphics overlay
        GraphicsOverlay {
            id: graphicsOverlay

            // original multipart red graphic
            Graphic {
                id: originalMultipointGraphic
                zIndex: 0

                SimpleMarkerSymbol {
                    color: "red"
                    size: 7
                    style: Enums.SimpleMarkerSymbolStyleCircle
                }

                Component.onCompleted: {
                    var pointCollection = createPointCollection();
                    var multipointBuilder = ArcGISRuntimeEnvironment.createObject("MultipointBuilder", { spatialReference: pointCollection.spatialReference });
                    multipointBuilder.points = pointCollection;
                    originalMultipointGraphic.geometry = multipointBuilder.geometry;
                    mapView.setViewpointGeometryAndPadding(originalMultipointGraphic.geometry, 100);
                }
            }

            // original red dotted line graphic
            Graphic {
                id: originalLineGraphic
                zIndex: 1

                SimpleLineSymbol {
                    color: "red"
                    style: Enums.SimpleLineSymbolStyleDot
                    width: 3
                }

                Component.onCompleted: {
                    var pointCollection = createPointCollection();
                    var polylineBuilder = ArcGISRuntimeEnvironment.createObject("PolylineBuilder", { spatialReference: pointCollection.spatialReference });
                    for (var i = 0; i < pointCollection.size; i++) {
                        polylineBuilder.addPoint(pointCollection.point(i));
                    }
                    originalLineGraphic.geometry = polylineBuilder.geometry;
                }
            }

            // resulting (densified and generalized) multipart magenta graphic
            Graphic {
                id: resultMultipointGraphic
                visible: showResultCheckbox.checked
                zIndex: 2

                SimpleMarkerSymbol {
                    color: "magenta"
                    size: 7
                    style: Enums.SimpleMarkerSymbolStyleCircle
                }
            }

            // resulting (densified and generalized) multipart magenta graphic
            Graphic {
                id: resultPolylineGraphic
                visible: showResultCheckbox.checked
                zIndex: 3

                SimpleLineSymbol {
                    color: "magenta"
                    width: 3
                    style: Enums.SimpleLineSymbolStyleSolid
                }
            }
        }
    }

    function updateGeometry() {
        // Get the initial Geometry
        var polyline = originalLineGraphic.geometry;
        if (!polyline)
            return;

        // Generalize the polyline
        if (generalizeCheckbox.checked)
            polyline = GeometryEngine.generalize(polyline, maxDeviationSlider.value, true);

        // Densify the polyline
        if (densifyCheckbox.checked)
            polyline = GeometryEngine.densify(polyline, maxSegmentLengthSlider.value);

        // Update the line graphic
        resultPolylineGraphic.geometry = polyline;

        // Update the multipoint graphic
        var multipointBuilder = ArcGISRuntimeEnvironment.createObject("MultipointBuilder", { spatialReference: polyline.spatialReference });
        var pointCollection = ArcGISRuntimeEnvironment.createObject("PointCollection", { spatialReference: polyline.spatialReference });
        if (polyline.parts.size < 1)
            return;

        var polylinePoints = polyline.parts.part(0).points();
        for (var i = 0; i < polylinePoints.size; i++) {
            pointCollection.addPoint(polylinePoints.point(i));
        }
        multipointBuilder.points = pointCollection;
        resultMultipointGraphic.geometry = multipointBuilder.geometry;
    }

    function createPointCollection() {
        var sr = ArcGISRuntimeEnvironment.createObject("SpatialReference", {wkid: 32126});
        var pointCollection = ArcGISRuntimeEnvironment.createObject("PointCollection", { spatialReference: sr });
        pointCollection.addPointXYZ(2330611.130549, 202360.002957, 0.000000);
        pointCollection.addPointXYZ(2330583.834672, 202525.984012, 0.000000);
        pointCollection.addPointXYZ(2330574.164902, 202691.488009, 0.000000);
        pointCollection.addPointXYZ(2330689.292623, 203170.045888, 0.000000);
        pointCollection.addPointXYZ(2330696.773344, 203317.495798, 0.000000);
        pointCollection.addPointXYZ(2330691.419723, 203380.917080, 0.000000);
        pointCollection.addPointXYZ(2330435.065296, 203816.662457, 0.000000);
        pointCollection.addPointXYZ(2330369.500800, 204329.861789, 0.000000);
        pointCollection.addPointXYZ(2330400.929891, 204712.129673, 0.000000);
        pointCollection.addPointXYZ(2330484.300447, 204927.797132, 0.000000);
        pointCollection.addPointXYZ(2330514.469919, 205000.792463, 0.000000);
        pointCollection.addPointXYZ(2330638.099138, 205271.601116, 0.000000);
        pointCollection.addPointXYZ(2330725.315888, 205631.231308, 0.000000);
        pointCollection.addPointXYZ(2330755.640702, 206433.354860, 0.000000);
        pointCollection.addPointXYZ(2330680.644719, 206660.240923, 0.000000);
        pointCollection.addPointXYZ(2330386.957926, 207340.947204, 0.000000);
        pointCollection.addPointXYZ(2330485.861737, 207742.298501, 0.000000);
        return pointCollection;
    }


    Rectangle {
        anchors {
            fill: controlColumn
            margins: -5
        }
        color: "#f9f9f9"
        radius: 5
    }

    Column {
        id: controlColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        spacing: 5


        CheckBox {
            id: densifyCheckbox
            text: "Densify"
            checked: true
        }

        Text {
            text: "Max segment length"
            enabled: densifyCheckbox.checked
        }

        Slider {
            id: maxSegmentLengthSlider
            from: 100
            to: 500
            width: 175
            value: 100
            onValueChanged: updateGeometry();
        }

        CheckBox {
            id: generalizeCheckbox
            text: "Generalize"
            checked: true
        }

        Text {
            text: "Max deviation"
            enabled: generalizeCheckbox.checked
        }

        Slider {
            id: maxDeviationSlider
            from: 1
            to: 250
            width: 175
            onValueChanged: updateGeometry();
        }

        CheckBox {
            id: showResultCheckbox
            text: "Show Result"
            checked: true
        }
    }
}
