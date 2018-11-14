// [WriteFile Name=GOSymbols, Category=DisplayInformation]
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
import Esri.ArcGISRuntime 100.5

Rectangle {
    width: 800
    height: 600

    property var buoyLocArray: []
    property var symbolTextArray: ["Craigleith", "Bass Rock"]
    property var textLocArray: []

    //! [GOSymbols MapView]
    // Map view UI presentation at top
    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapOceans {}
            initialViewpoint: viewPoint
        }

        GraphicsOverlay {
            id: graphicsOverlay
        }
    }
    //! [GOSymbols MapView]

    // the center of the mapview
    ViewpointCenter {
        id: viewPoint
        center: Point {
            x: -2.681572
            y: 56.075844
            spatialReference: SpatialReference {
                wkid: 4326
            }
        }
        targetScale: 288895.277144
    }

    //! [GOSymbol buoy points]
    // buoy location 1 (immutable once created)
    Point {
        x: -2.712642647560347
        y: 56.062812566811544
        spatialReference: SpatialReference.createWgs84()

        onComponentCompleted: {
            buoyLocArray.push(this);
        }
    }

    // buoy location 2 (immutable once created)
    Point {
        x: -2.6908416959572303
        y: 56.06444173689877
        spatialReference: SpatialReference.createWgs84()

        onComponentCompleted: {
            buoyLocArray.push(this);
        }
    }

    // buoy location 3 (immutable once created)
    Point {
        x: -2.6697273884990937
        y: 56.064250073402874
        spatialReference: SpatialReference.createWgs84()

        onComponentCompleted: {
            buoyLocArray.push(this);
        }
    }

    // buoy location 4 (immutable once created)
    Point {
        x: -2.6395150461199726
        y: 56.06127916736989
        spatialReference: SpatialReference.createWgs84()

        onComponentCompleted: {
            buoyLocArray.push(this);
        }
    }
    //! [GOSymbol buoy points]

    //! [GOSymbol buoy symbol]
    // buoy symbol
    SimpleMarkerSymbol {
        id: buoyMarkerSymbol
        style: Enums.SimpleMarkerSymbolStyleCircle
        color: "red"
        size: 10
    }
    //! [GOSymbol buoy symbol]

    //! [GOSymbol text points]
    // text location 1: craigleith (immutable once created)
    Point {
        x: -2.640631
        y: 56.078083
        spatialReference: SpatialReference.createWgs84()

        onComponentCompleted: {
            textLocArray.push(this);
        }
    }

    // text location 2: bass rock (immutable once created)
    Point {
        x: -2.720324
        y: 56.073569
        spatialReference: SpatialReference.createWgs84()

        onComponentCompleted: {
            textLocArray.push(this);
        }
    }
    //! [GOSymbol text points]


    //! [GOSymbol line symbol]
    // LineSymbol
    SimpleLineSymbol {
        id: boatRouteSymbol
        style: Enums.SimpleLineSymbolStyleDash
        color: Qt.rgba(0, 0, 1, 0.7)
        width: 1
        antiAlias: true
    }
    //! [GOSymbol line symbol]

    //! [GOSymbol nesting ground]
    // polygon for nesting ground
    PolygonBuilder {
        id: nestingGroundPolygonBuilder
        spatialReference: SpatialReference.createWgs84()
    }

    // symbol for nesting ground
    SimpleFillSymbol {
        id: nestingGroundSymbol
        style: Enums.SimpleFillSymbolStyleDiagonalCross
        color: Qt.rgba(0.0, 0.31, 0.0, 1)

        // default property: ouline
        SimpleLineSymbol {
            style: Enums.SimpleLineSymbolStyleDash
            color: Qt.rgba(0.0, 0.0, 0.5, 1)
            width: 1
            antiAlias: true
        }
    }
    //! [GOSymbol nesting ground]

    // add all the graphics to the graphics overlay
    // create each graphic type and then add them to the overlay at a time
    // alternatively you can create all of them and store in a list and add all of them
    // by calling appendAll and pass in the list.
    Component.onCompleted: {
        // add buoy points
        buoyLocArray.forEach(function(buoyPoint) {
            graphicsOverlay.graphics.append(createGraphic(buoyPoint, buoyMarkerSymbol));
        });

        // add text symbols
        for (var i = 0; i < textLocArray.length; i++) {
            var textSymbol = ArcGISRuntimeEnvironment.createObject("TextSymbol");
            textSymbol.size = 10;
            textSymbol.color = Qt.rgba(0.1, 0.4, 0.9, 1);
            textSymbol.text = symbolTextArray[i];

            if (i === 1) {
                textSymbol.horizontalAlignment = Enums.HorizontalAlignmentLeft;
                textSymbol.verticalAlignment = Enums.VerticalAlignmentBottom;
            } else {
                textSymbol.horizontalAlignment =  Enums.HorizontalAlignmentRight;
                textSymbol.verticalAlignment =  Enums.VerticalAlignmentTop;
            }
            graphicsOverlay.graphics.append(createGraphic(textLocArray[i], textSymbol));
        }

        // add boat route
        graphicsOverlay.graphics.append(createGraphic(createBoatRoute(), boatRouteSymbol));

        // add the nesting ground
        graphicsOverlay.graphics.append(createGraphic(createNestingGround(), nestingGroundSymbol));
    }

    //! [GOSymbol createGraphic]
    // create and return a graphic
    function createGraphic(geometry, symbol) {
        var graphic = ArcGISRuntimeEnvironment.createObject("Graphic");
        graphic.geometry = geometry;
        graphic.symbol = symbol;
        return graphic;
    }
    //! [GOSymbol createGraphic]

    //! [GOSymbol createBoatRoute]
    // create the boat route
    function createBoatRoute() {
        // create polyline using json
        return ArcGISRuntimeEnvironment.createObject("Polyline", {"json": boatRouteJson});
    }
    //! [GOSymbol createBoatRoute]

    //! [GOSymbol createNestingGround]
    // create the nesting ground polygon
    function createNestingGround() {
        // create the polygon using the builder class
        nestingGroundPolygonBuilder.addPointXY(-2.643077012566659, 56.077125346044475);
        nestingGroundPolygonBuilder.addPointXY(-2.6428195210159444, 56.07717324600376);
        nestingGroundPolygonBuilder.addPointXY(-2.6425405718360033, 56.07774804087097);
        nestingGroundPolygonBuilder.addPointXY(-2.6427122328698127, 56.077927662508635);
        nestingGroundPolygonBuilder.addPointXY(-2.642454741319098, 56.07829887790651);
        nestingGroundPolygonBuilder.addPointXY(-2.641853927700763, 56.078526395253725);
        nestingGroundPolygonBuilder.addPointXY(-2.6409741649024867, 56.078801809192434);
        nestingGroundPolygonBuilder.addPointXY(-2.6399871139580795, 56.07881378366685);
        nestingGroundPolygonBuilder.addPointXY(-2.6394077579689705, 56.07908919555142);
        nestingGroundPolygonBuilder.addPointXY(-2.638764029092183, 56.07917301616904);
        nestingGroundPolygonBuilder.addPointXY(-2.638485079912242, 56.07896945149566);
        nestingGroundPolygonBuilder.addPointXY(-2.638570910429147, 56.078203080726844);
        nestingGroundPolygonBuilder.addPointXY(-2.63878548672141, 56.077568418396);
        nestingGroundPolygonBuilder.addPointXY(-2.6391931816767085, 56.077197195961084);
        nestingGroundPolygonBuilder.addPointXY(-2.6399441986996273, 56.07675411934114);
        nestingGroundPolygonBuilder.addPointXY(-2.6406523004640934, 56.076730169108444);
        nestingGroundPolygonBuilder.addPointXY(-2.6406737580933193, 56.07632301287509);
        nestingGroundPolygonBuilder.addPointXY(-2.6401802326211157, 56.075999679860494);
        nestingGroundPolygonBuilder.addPointXY(-2.6402446055087943, 56.075844000034046);
        nestingGroundPolygonBuilder.addPointXY(-2.640416266542604, 56.07578412301025);
        nestingGroundPolygonBuilder.addPointXY(-2.6408883343855822, 56.075808073830935);
        nestingGroundPolygonBuilder.addPointXY(-2.6417680971838577, 56.076239186057734);
        nestingGroundPolygonBuilder.addPointXY(-2.642197249768383, 56.076251161328514);
        nestingGroundPolygonBuilder.addPointXY(-2.6428409786451708, 56.07661041772168);
        nestingGroundPolygonBuilder.addPointXY(-2.643077012566659, 56.077125346044475);

        return nestingGroundPolygonBuilder.geometry;
    }
    //! [GOSymbol createNestingGround]

    //! [GOSymbol boatRouteJson]
    property var boatRouteJson : {"paths":[[[-2.7184791227926772,56.06147084563517],
                                    [-2.7196807500463924,56.06147084563517],
                                    [-2.722084004553823,56.062141712059706],
                                    [-2.726375530459948,56.06386674355254],
                                    [-2.726890513568683,56.0660708381432],
                                    [-2.7270621746049275,56.06779569383808],
                                    [-2.7255172252787228,56.068753913653914],
                                    [-2.723113970771293,56.069424653352335],
                                    [-2.719165766937657,56.07028701581465],
                                    [-2.713672613777817,56.070574465681325],
                                    [-2.7093810878716917,56.07095772883556],
                                    [-2.7044029178205866,56.07153261642126],
                                    [-2.698223120515766,56.072394931722265],
                                    [-2.6923866452834355,56.07325722773041],
                                    [-2.68672183108735,56.07335303720707],
                                    [-2.6812286779275096,56.07354465544585],
                                    [-2.6764221689126497,56.074215311778964],
                                    [-2.6698990495353394,56.07488595644139],
                                    [-2.6647492184479886,56.075748196715914],
                                    [-2.659427726324393,56.076131408423215],
                                    [-2.654792878345778,56.07622721075461],
                                    [-2.651359657620878,56.076514616319784],
                                    [-2.6477547758597324,56.07708942101955],
                                    [-2.6450081992798125,56.07814320736718],
                                    [-2.6432915889173625,56.08025069360931],
                                    [-2.638656740938747,56.08044227755186],
                                    [-2.636940130576297,56.078813783674946],
                                    [-2.636425147467562,56.07728102068079],
                                    [-2.637798435757522,56.076610417698504],
                                    [-2.638656740938747,56.07507756705851],
                                    [-2.641231656482422,56.07479015077557],
                                    [-2.6427766058086277,56.075748196715914],
                                    [-2.6456948434247924,56.07546078543464],
                                    [-2.647239792750997,56.074598538729404],
                                    [-2.6492997251859376,56.072682365868616],
                                    [-2.6530762679833284,56.0718200569986],
                                    [-2.655479522490758,56.070861913404286],
                                    [-2.6587410821794135,56.07047864929729],
                                    [-2.6633759301580286,56.07028701581465],
                                    [-2.666637489846684,56.07009538137926],
                                    [-2.670070710571584,56.06990374599109],
                                    [-2.6741905754414645,56.069137194910745],
                                    [-2.678310440311345,56.06808316228391],
                                    [-2.682086983108735,56.06789151689155],
                                    [-2.6868934921235956,56.06760404701653],
                                    [-2.6911850180297208,56.06722075051504],
                                    [-2.695133221863356,56.06702910083509],
                                    [-2.698223120515766,56.066837450202335],
                                    [-2.7016563412406667,56.06645414607839],
                                    [-2.7061195281830366,56.0660708381432],
                                    [-2.7100677320166717,56.065591697864576],
                                    [-2.713329291705327,56.06520838135397],
                                    [-2.7167625124302273,56.06453756828941],
                                    [-2.718307461756433,56.06348340989081],
                                    [-2.719165766937657,56.062812566811544],
                                    [-2.7198524110826376,56.06204587471371],
                                    [-2.719165766937657,56.06166252294756],
                                    [-2.718307461756433,56.06147084563517]]],
                                    "spatialReference":{"wkid":4326}}
    //! [GOSymbol boatRouteJson]
}
