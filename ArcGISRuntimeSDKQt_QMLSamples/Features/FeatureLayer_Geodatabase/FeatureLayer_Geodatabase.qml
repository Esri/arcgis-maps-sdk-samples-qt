// [WriteFile Name=FeatureLayer_Geodatabase, Category=Features]
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

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/"

    // Map view UI presentation at top
    MapView {
        anchors.fill: parent

        Map {
            id: map

            // set an initial viewpoint
            ViewpointCenter {
                Point {
                    x: -13214155
                    y: 4040194
                    spatialReference: SpatialReference.createWebMercator()
                }
                scale: 35e4
            }

            // create a basemap from a local vector tile package
            Basemap {
                ArcGISVectorTiledLayer {
                    url: System.resolvedPathUrl(dataPath + "LosAngeles.vtpk")
                }
            }

            // create a feature layer
            FeatureLayer {
                // obtain the feature table from the geodatabase by name
                featureTable: gdb.geodatabaseFeatureTablesByTableName["Trailheads"]

                // create the geodatabase
                Geodatabase {
                    id: gdb
                    path: dataPath + "LA_Trails.geodatabase"
                }
            }
        }
    }

    // Neatline rectangle
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
