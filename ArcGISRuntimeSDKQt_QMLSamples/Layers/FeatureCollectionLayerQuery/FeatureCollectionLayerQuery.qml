// [WriteFile Name=FeatureCollectionLayerQuery, Category=Layers]
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
import Esri.ArcGISRuntime 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property bool busy: featureTable.queryFeaturesStatus === Enums.TaskStatusInProgress

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            BasemapOceans {}
        }
    }

    ServiceFeatureTable {
        id: featureTable
        url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/Wildfire/FeatureServer/0"

        onQueryFeaturesStatusChanged: {
            if (queryFeaturesStatus !== Enums.TaskStatusCompleted)
                return;

            const featureCollectionTable = ArcGISRuntimeEnvironment.createObject("FeatureCollectionTable", {featureSet: queryFeaturesResult});

            const featureCollection = ArcGISRuntimeEnvironment.createObject("FeatureCollection");
            featureCollection.tables.append(featureCollectionTable);

            featureCollectionLayer.featureCollection = featureCollection;
        }

        Component.onCompleted: {
            queryFeatures(queryParams);
        }
    }

    QueryParameters {
        id: queryParams
        whereClause: "1=1"
    }

    FeatureCollectionLayer {
        id: featureCollectionLayer

        onFeatureCollectionChanged: {
            map.operationalLayers.append(featureCollectionLayer);
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        visible: busy
    }
}
