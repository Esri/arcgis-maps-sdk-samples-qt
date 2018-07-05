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

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Esri.ArcGISRuntime 100.3
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/"
    property string errorMessage: ""

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
                    spatialReference: SpatialReference { wkid: 3857 }
                }
                targetScale: 35e4
            }

            //! [FeatureLayer Geodatabase add basemap]
            // create a basemap from a local vector tile package
            Basemap {
                ArcGISVectorTiledLayer {
                    url: dataPath + "vtpk/LosAngeles.vtpk"
                }
            }
            //! [FeatureLayer Geodatabase add basemap]

            //! [FeatureLayer Geodatabase create]
            // create a feature layer
            FeatureLayer {
                // obtain the feature table from the geodatabase by name
                featureTable: gdb.geodatabaseFeatureTablesByTableName["Trailheads"] ?
                                  gdb.geodatabaseFeatureTablesByTableName["Trailheads"] :
                                  null

                // create the geodatabase
                Geodatabase {
                    id: gdb
                    path: dataPath + "geodatabase/LA_Trails.geodatabase"

                    onErrorChanged: errorMessage = error.message;
                }

                onErrorChanged: errorMessage = error.message;
            }
            //! [FeatureLayer Geodatabase create]

            onErrorChanged: errorMessage = error.message;
        }

        onErrorChanged: errorMessage = error.message;
    }

    MessageDialog {
        visible: text.length > 0
        text: errorMessage
        informativeText: "Please consult the README.md"
    }
}
