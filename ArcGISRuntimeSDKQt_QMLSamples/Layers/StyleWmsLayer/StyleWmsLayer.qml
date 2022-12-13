// [WriteFile Name=StyleWmsLayer, Category=Layers]
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

import QtQuick
import QtQuick.Controls
import Esri.ArcGISRuntime

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property var sublayer: null
    property var styles

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            // create a map with spatial reference appropriate for the WMS service (North American Datum 83)
            spatialReference: SpatialReference {wkid: 26915}
            minScale: 7000000

            WmsLayer {
                id: wmsLayer
                url: "https://imageserver.gisdata.mn.gov/cgi-bin/mncomp?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetCapabilities"
                layerNames: ["mncomp"]

                onLoadStatusChanged: {
                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    // zoom to the layer
                    mapView.setViewpointGeometry(fullExtent);

                    // get the subLayer
                    sublayer = sublayers[0];

                    // get the styles
                    styles = sublayer.sublayerInfo.styles;
                }
            }
        }
    }

    Rectangle {
        anchors {
            fill: controlColumn
            margins: -5
        }
        color: "lightgray"
        radius: 5
        opacity: 0.75
    }

    ButtonGroup {
        buttons: controlColumn.children
    }

    Column {
        id: controlColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        spacing: 5

        RadioButton {
            text: "Default"
            checked: true
            onCheckedChanged: {
                if (checked) {
                    if (!sublayer)
                        return;

                    // set the style string on the sublayer
                    sublayer.currentStyle = styles[0];
                }
            }
        }

        RadioButton {
            text: "Contrast Stretch"
            onCheckedChanged: {
                if (checked) {
                    if (!sublayer)
                        return;

                    // set the style string on the sublayer
                    sublayer.currentStyle = styles[1];
                }
            }
        }
    }
}
