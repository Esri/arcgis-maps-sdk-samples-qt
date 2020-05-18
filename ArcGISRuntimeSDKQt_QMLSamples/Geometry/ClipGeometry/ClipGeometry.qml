// [WriteFile Name=ClipGeometry, Category=Geometry]
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
import Esri.ArcGISRuntime 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600    

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapTopographic {}
            initialViewpoint: vc

            // Create the envelope for Colorado
            Envelope {
                id: coloradoGeometry
                xMax: -11362327.128340
                xMin: -12138232.018408
                yMin: 4441198.773776
                yMax: 5012861.290274
                spatialReference: SpatialReference {
                    wkid: 3857
                }
            }

            // Create the dotted outline for the clipping envelopes
            SimpleLineSymbol {
                id: redOutline
                style: Enums.SimpleLineSymbolStyleDot
                color: "#FFFF0000"
                width: 3
            }

            // Create the outline and fill for Colorado
            SimpleFillSymbol {
                id: coloradoFillSymbol
                color: "#220000FF"
                style: Enums.SimpleFillSymbolStyleSolid
                SimpleLineSymbol {
                    color: "#FF0000FF"
                    width: 2.0
                    style: Enums.SimpleFillSymbolStyleSolid
                }
            }
        }

        ViewpointCenter {
            id: vc
            center: Point {
                x: -11655182.595204
                y: 4741618.772994
                spatialReference: SpatialReference {
                    wkid: 3857
                }
            }
            targetScale: 15151632
        }

        // Create an envelope outside of Colorado
        Envelope {
            id: outsideEnvelope
            xMax: -11858344.321294
            xMin: -12201990.219681
            yMax: 5297071.577304
            yMin: 5147942.225174
            spatialReference: SpatialReference {
                wkid: 3857
            }
        }

        // Create an envelope intersecting Colorado
        Envelope {
            id: intersectingEnvelope
            xMax: -11962086.479298
            xMin: -12260345.183558
            yMax: 4566553.881363
            yMin: 4332053.378376
            spatialReference: SpatialReference {
                wkid: 3857
            }
        }

        // Create an envelope inside of Colorado
        Envelope {
            id: containedEnvelope
            xMax: -11431488.567009
            xMin: -11655182.595204
            yMax: 4741618.772994
            yMin: 4593570.068343
            spatialReference: SpatialReference {
                wkid: 3857
            }
        }

        // Create a graphics overlay for the Colorado geometry
        GraphicsOverlay {
            id: coloradoOverlay

            // Colorado
            Graphic {
                id: coloradoGraphic
                geometry: coloradoGeometry
                symbol: coloradoFillSymbol
            }
        }

        // Create a graphics overlay to contain the clipping envelopes
        GraphicsOverlay {
            id: envelopesOverlay

            // Outside envelope
            Graphic {
                id: outsideEnvelopeGraphic
                geometry: outsideEnvelope
                symbol: redOutline
            }

            // Intersecting envelope
            Graphic {
                id: intersectingEnvelopeGraphic
                geometry: intersectingEnvelope
                symbol: redOutline
            }

            // Contained envelope
            Graphic {
                id: containedEnvelopeGraphic
                geometry: containedEnvelope
                symbol: redOutline
            }
        }

        // Create a graphics overlay for the clipped graphics
        GraphicsOverlay {
            id: clippedAreasOverlay
        }
    }

    // Create a button that clips the geometry into the envelopes
    Button {
        id: clipButton
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 25
        }
        text: "Clip"
        onClicked: {
            // Immediately hide the Colorado graphic to prevent overlap
            coloradoOverlay.visible = false;

            // Iterate through the clipping envelopes
            envelopesOverlay.graphics.forEach(graphic => {

                // Create a variable that contains the clip result, which is an envelope of the overlap between colorado and the current graphic
                const clippedGeometry = GeometryEngine.clip(coloradoGraphic.geometry, graphic.geometry.extent);
                if (clippedGeometry !== null) {

                    // Create a new graphic using the clip envelope, and fill it in with the colorado fill symbol
                    const clippedGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", { geometry: clippedGeometry, symbol: coloradoFillSymbol });

                    // Add the new clipped graphic to the map
                    clippedAreasOverlay.graphics.append(clippedGraphic);
                }
            });

            // Only allow the clip action to fire once
            clipButton.enabled = false;
        }
    }
}
