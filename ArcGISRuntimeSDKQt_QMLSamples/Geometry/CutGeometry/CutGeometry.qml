// [WriteFile Name=CutGeometry, Category=Geometry]
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
    clip: true
    width: 800
    height: 600    

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapTopographic {}

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    borderGraphic.geometry = createBorder();
                    lakeSuperiorGraphic.geometry = createLakeSuperiorPolygon();
                    mapView.setViewpointGeometry(lakeSuperiorGraphic.geometry);
                }
            }
        }

        // Display Graphics of Lake Superior and the U.S./Canada border
        GraphicsOverlay {
            id: graphicsOverlay

            // Lake Superior
            Graphic {
                id: lakeSuperiorGraphic

                SimpleFillSymbol {
                    style: Enums.SimpleFillSymbolStyleSolid
                    color: Qt.rgba(0.78, 0.8, 1, 0.7)

                    // outline
                    SimpleLineSymbol {
                        style: Enums.SimpleLineSymbolStyleSolid
                        color: "blue"
                        width: 1
                    }
                }
            }

            // U.S. / Canada Border
            Graphic {
                id: borderGraphic
                zIndex: 10

                SimpleLineSymbol {
                    style: Enums.SimpleLineSymbolStyleDot
                    color: "red"
                    width: 5
                }
            }
        }
    }

    Button {
        id: cutButton
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }

        text: "Cut"
        onClicked: cutPolygon();
    }

    SpatialReference {
        id: spatialRef
        wkid: 3857
    }

    // Cuts the Lake Superior polygon into two and symbolizes each output graphic
    function cutPolygon() {
        // cut the polygon geometry with the polyline, expect two geometries
        var geoms = GeometryEngine.cut(lakeSuperiorGraphic.geometry, borderGraphic.geometry);

        // create graphics for the U.S. side
        var usSide = ArcGISRuntimeEnvironment.createObject("Graphic");
        usSide.geometry = geoms[0];
        var usSymbol = ArcGISRuntimeEnvironment.createObject("SimpleFillSymbol");
        usSymbol.style = Enums.SimpleFillSymbolStyleForwardDiagonal;
        usSymbol.color = "#40ff00";
        var outlineSymbol = ArcGISRuntimeEnvironment.createObject("SimpleLineSymbol");
        outlineSymbol.color = "blue";
        outlineSymbol.width = 1;
        usSymbol.outline = outlineSymbol;
        usSide.symbol = usSymbol;

        // create graphics for the Canada side
        var canadaSide = ArcGISRuntimeEnvironment.createObject("Graphic");
        canadaSide.geometry = geoms[1];
        var canadaSymbol = ArcGISRuntimeEnvironment.createObject("SimpleFillSymbol");
        canadaSymbol.style = Enums.SimpleFillSymbolStyleForwardDiagonal;
        canadaSymbol.color = "#ffff00";
        canadaSymbol.outline = outlineSymbol;
        canadaSide.symbol = canadaSymbol;

        // add graphics
        graphicsOverlay.graphics.append(canadaSide);
        graphicsOverlay.graphics.append(usSide);

        // disable button (only cut once)
        cutButton.enabled = false;
    }

    // Creates a line between the border of Canada and the United States
    function createBorder() {
        var polylineBuilder = ArcGISRuntimeEnvironment.createObject("PolylineBuilder", { spatialReference: spatialRef });
        polylineBuilder.addPointXY(-9981328.687124, 6111053.281447);
        polylineBuilder.addPointXY(-9946518.044066, 6102350.620682);
        polylineBuilder.addPointXY(-9872545.427566, 6152390.920079);
        polylineBuilder.addPointXY(-9838822.617103, 6157830.083057);
        polylineBuilder.addPointXY(-9446115.050097, 5927209.572793);
        polylineBuilder.addPointXY(-9430885.393759, 5876081.440801);
        polylineBuilder.addPointXY(-9415655.737420, 5860851.784463);
        return polylineBuilder.geometry;
    }

    // Creates a polygon of Lake Superior
    function createLakeSuperiorPolygon() {
        var polygonBuilder = ArcGISRuntimeEnvironment.createObject("PolygonBuilder", { spatialReference: spatialRef });
        polygonBuilder.addPointXY(-10254374.668616, 5908345.076380);
        polygonBuilder.addPointXY(-10178382.525314, 5971402.386779);
        polygonBuilder.addPointXY(-10118558.923141, 6034459.697178);
        polygonBuilder.addPointXY(-9993252.729399, 6093474.872295);
        polygonBuilder.addPointXY(-9882498.222673, 6209888.368416);
        polygonBuilder.addPointXY(-9821057.766387, 6274562.532928);
        polygonBuilder.addPointXY(-9690092.583250, 6241417.023616);
        polygonBuilder.addPointXY(-9605207.742329, 6206654.660191);
        polygonBuilder.addPointXY(-9564786.389509, 6108834.986367);
        polygonBuilder.addPointXY(-9449989.747500, 6095091.726408);
        polygonBuilder.addPointXY(-9462116.153346, 6044160.821855);
        polygonBuilder.addPointXY(-9417652.665244, 5985145.646738);
        polygonBuilder.addPointXY(-9438671.768711, 5946341.148031);
        polygonBuilder.addPointXY(-9398250.415891, 5922088.336339);
        polygonBuilder.addPointXY(-9419269.519357, 5855797.317714);
        polygonBuilder.addPointXY(-9467775.142741, 5858222.598884);
        polygonBuilder.addPointXY(-9462924.580403, 5902686.086985);
        polygonBuilder.addPointXY(-9598740.325877, 5884092.264688);
        polygonBuilder.addPointXY(-9643203.813979, 5845287.765981);
        polygonBuilder.addPointXY(-9739406.633691, 5879241.702350);
        polygonBuilder.addPointXY(-9783061.694736, 5922896.763395);
        polygonBuilder.addPointXY(-9844502.151022, 5936640.023354);
        polygonBuilder.addPointXY(-9773360.570059, 6019099.583107);
        polygonBuilder.addPointXY(-9883306.649729, 5968977.105610);
        polygonBuilder.addPointXY(-9957681.938918, 5912387.211662);
        polygonBuilder.addPointXY(-10055501.612742, 5871965.858842);
        polygonBuilder.addPointXY(-10116942.069028, 5884092.264688);
        polygonBuilder.addPointXY(-10111283.079633, 5933406.315128);
        polygonBuilder.addPointXY(-10214761.742852, 5888134.399970);
        polygonBuilder.addPointXY(-10254374.668616, 5901877.659929);
        return polygonBuilder.geometry;
    }
}
