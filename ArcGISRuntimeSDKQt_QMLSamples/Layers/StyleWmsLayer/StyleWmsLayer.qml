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

import QtQuick 2.6
import QtQuick.Controls 1.4
import Esri.ArcGISRuntime 100.3
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property var sublayer: null
    property var styles

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapImagery {}

            WmsLayer {
                id: wmsLayer
                url: "http://geoint.lmic.state.mn.us/cgi-bin/wms?VERSION=1.3.0&SERVICE=WMS&REQUEST=GetCapabilities"
                layerNames: ["fsa2017"]

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
            margins: -5 * scaleFactor
        }
        color: "lightgray"
        radius: 5 * scaleFactor
        opacity: 0.75
    }

    Column {
        id: controlColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 10 * scaleFactor
        }
        spacing: 5 * scaleFactor

        RadioButton {
            text: "Style 1"
            exclusiveGroup: radioGroup
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
            text: "Style 2"
            exclusiveGroup: radioGroup
            onCheckedChanged: {
                if (checked) {
                    if (!sublayer)
                        return;

                    // set the style string on the sublayer
                    sublayer.currentStyle = styles[1];
                }
            }
        }

        ExclusiveGroup {
            id: radioGroup
        }
    }
}
