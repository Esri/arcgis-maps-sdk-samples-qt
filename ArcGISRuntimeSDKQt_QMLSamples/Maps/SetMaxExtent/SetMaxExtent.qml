// [WriteFile Name=SetMaxExtent, Category=Maps]
// [Legal]
// Copyright 2022 Esri.

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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.ArcGISRuntime

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property Envelope nullEnvelope: null

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            id: map
            initBasemapStyle: Enums.BasemapStyleArcGISStreets
            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    mapView.setViewpoint(viewpointExtent);
                    map.maxExtent = coloradoEnvelope;
                }
            }
            Envelope {
                id: coloradoEnvelope
                xMax: -11362327
                xMin: -12138232
                yMin: 4441198
                yMax: 5012861
                spatialReference: SpatialReference {
                    wkid: 3857
                }
            }
            ViewpointExtent {
                id: viewpointExtent
                extent: Envelope {
                    xMax: -11362327
                    xMin: -12138232
                    yMin: 4441198
                    yMax: 5012861
                    spatialReference: SpatialReference {
                        wkid: 3857
                    }
                }
            }

            // Create the dotted outline for the clipping envelopes
            SimpleLineSymbol {
                id: redOutline
                style: Enums.SimpleLineSymbolStyleDash
                color: "#FFFF0000"
                width: 3
            }
        }
        GraphicsOverlay {
            id: coloradoOverlay
            Graphic {
                id: coloradoGraphic
                geometry: coloradoEnvelope
                symbol: redOutline
            }
        }
    }
    Rectangle {
        anchors {
            margins: 5
            left: parent.left
            bottom: parent.bottom
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#D3D3D3"
        opacity: .75
        radius: 3

        ColumnLayout {
            Text {
                color: "#000000"
                text: "Max extent enabled"
                Layout.fillWidth: true
                Layout.margins: 3
                font {
                    weight: Font.DemiBold
                    pointSize: 10
                }
            }
            Switch {
                id: maxExtentSwitch
                checked: true
                onCheckedChanged: {
                    if (checked) {
                        mapView.setViewpoint(viewpointExtent);
                        map.maxExtent = coloradoEnvelope;
                    }
                    else {
                        map.maxExtent = nullEnvelope;
                    }
                }
            }
        }
    }
}
