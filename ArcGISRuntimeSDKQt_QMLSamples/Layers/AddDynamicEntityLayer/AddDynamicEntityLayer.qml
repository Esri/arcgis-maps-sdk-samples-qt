// [WriteFile Name=AddDynamicEntityLayer, Category=Layers]
// [Legal]
// Copyright 2023 Esri.

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
import Esri.ArcGISRuntime

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            initBasemapStyle: Enums.BasemapStyleArcGISDarkGray

            initialViewpoint: ViewpointExtent {
                extent: sandyUtahEnvelope
            }

            // A layer that displays dynamic entities from real time services
            DynamicEntityLayer {
                id: dynamicEntityLayer

                dataSource: ArcGISStreamService {
                    id: dynamicEntityDataSource
                    url: "https://realtimegis2016.esri.com:6443/arcgis/rest/services/SandyVehicles/StreamServer"
                    filter: ArcGISStreamServiceFilter {
                        // This filters what information is received from the server
                        whereClause: "speed > 0"
                        geometry: sandyUtahEnvelope
                    }

                    // Set the time in seconds after which observations will be removed from the cache
                    purgeOptions.maximumDuration: 300
                }

                // Set the renderer for the most recent observations
                renderer: UniqueValueRenderer {
                    fieldNames: ["agency"]

                    UniqueValue {
                        values: [3]
                        symbol: SimpleMarkerSymbol {
                            style: Enums.SimpleMarkerSymbolStyleCircle
                            color: "magenta"
                            size: 8
                        }
                    }
                    UniqueValue {
                        values: [4]
                        symbol: SimpleMarkerSymbol {
                            style: Enums.SimpleMarkerSymbolStyleCircle
                            color: "green"
                            size: 8
                        }
                    }

                    defaultSymbol: SimpleMarkerSymbol {
                        style: Enums.SimpleMarkerSymbolStyleCircle
                        color: "blue"
                        size: 8
                    }
                }

                // Set the display properties for previous observations and the trackline
                trackDisplayProperties {
                    showTrackLine: true
                    showPreviousObservations: true
                    previousObservationRenderer: UniqueValueRenderer {
                        fieldNames: ["agency"]

                        UniqueValue {
                            values: [3]
                            symbol: SimpleMarkerSymbol {
                                style: Enums.SimpleMarkerSymbolStyleCircle
                                color: "magenta"
                                size: 3
                            }
                        }
                        UniqueValue {
                            values: [4]
                            symbol: SimpleMarkerSymbol {
                                style: Enums.SimpleMarkerSymbolStyleCircle
                                color: "green"
                                size: 3
                            }
                        }

                        defaultSymbol: SimpleMarkerSymbol {
                            style: Enums.SimpleMarkerSymbolStyleCircle
                            color: "blue"
                            size: 3
                        }
                    }
                    trackLineRenderer: SimpleRenderer {
                        symbol: SimpleLineSymbol {
                            style: Enums.SimpleLineSymbolStyleSolid
                            color: "lightGray"
                            width: 2
                        }
                    }
                }
            }
        }

        GraphicsOverlay {
            id: sandyBorderOverlay
            Graphic {
                id: sandyBorderGraphic
                geometry: Envelope {
                    id: sandyUtahEnvelope
                    xMin: -112.110052
                    xMax: -111.814782
                    yMin: 40.535247
                    yMax: 40.718083
                    spatialReference: Factory.SpatialReference.createWgs84()
                }
                symbol: SimpleLineSymbol {
                    style: Enums.SimpleLineSymbolStyleDash
                    color: "red"
                    width: 2
                }
            }
        }
    }

    Rectangle {
        id: dynamicEntityLayerOptions
        width: (300 < parent.width - (anchors.margins * 2)) ? 300 : parent.width - (anchors.margins * 2)
        height: column.height + (column.anchors.margins * 2)
        border.width: 1
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10

        Column {
            id: column
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: 10
            }

            Text {
                id: statusText
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Status: " + ["Disconnected","Connecting","Connected"][dynamicEntityDataSource.connectionStatus]
                font.pixelSize: 18
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
            }

            Switch {
                id: statusSwitch
                anchors.horizontalCenter: parent.horizontalCenter
                checked: dynamicEntityDataSource.connectionStatus !== Enums.ConnectionStatusDisconnected
                onCheckedChanged: {
                    checked ? dynamicEntityDataSource.connectDataSource() : dynamicEntityDataSource.disconnectDataSource();
                }
            }

            CheckBox {
                id: trackLinesBox
                text: "Track lines"
                checked: true
                onCheckedChanged: {
                    // Show or hide the lines that connect previous observations
                    dynamicEntityLayer.trackDisplayProperties.showTrackLine = checked;
                }
            }


            CheckBox {
                id: previousObservationsBox
                text: "Previous observations"
                checked: true
                onCheckedChanged: {
                    // Show or hide previous observations (if maximum observations is greater than 1)
                    dynamicEntityLayer.trackDisplayProperties.showPreviousObservations = checked;
                }
            }

            Text {
                id: observationsSliderText
                topPadding: 10
                text: "Observations per track (" + observationsSlider.value + ")"
            }

            Slider {
                id: observationsSlider
                anchors {
                    left: parent.left
                    right: parent.right
                }
                value: 5
                stepSize: 1
                from: 1
                to: 16
                onValueChanged: {
                    // Update the number of entity observations displayed using the value from the Slider
                    dynamicEntityLayer.trackDisplayProperties.maximumObservations = value;
                }
            }

            Button {
                id: purgeButton
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Purge all observations"
                onClicked: {
                    // Remove all current observations from the cache
                    dynamicEntityDataSource.purgeAll();
                }
            }
        }
    }
}
