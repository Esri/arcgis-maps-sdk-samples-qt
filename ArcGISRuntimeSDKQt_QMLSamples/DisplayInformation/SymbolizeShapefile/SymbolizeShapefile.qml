// [WriteFile Name=SymbolizeShapefile, Category=DisplayInformation]
// [Legal]
// Copyright 2017 Esri.

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
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/shp/"
    

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapStreetsVector {}

            // Declare a FeatureLayer for displaying the shapefile
            FeatureLayer {
                id: featureLayer
                // Create a ShapefileFeatureTable pointing to the data location
                ShapefileFeatureTable {
                    path: dataPath + "Subdivisions.shp"
                }
            }

            // set an initial viewpoint
            ViewpointCenter {
                Point {
                    y: 4818336
                    x: -11662054
                    spatialReference: SpatialReference { wkid: 3857 }
                }
                targetScale: 200000
            }
        }
    }

    // Update the renderer on click
    Button {
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        text: "Change Renderer"
        onClicked: {
            // apply the new renderer
            featureLayer.renderer = simpleRenderer;
            visible = false;
        }
    }

    // Create a SimpleRenderer
    SimpleRenderer {
        id: simpleRenderer

        SimpleFillSymbol {
            color: "yellow"
            style: Enums.SimpleFillSymbolStyleSolid

            SimpleLineSymbol {
                color: "red"
                style: Enums.SimpleLineSymbolStyleSolid
                width: 1
            }
        }
    }
}
