// [WriteFile Name=FeatureCollectionLayerFromPortal, Category=Layers]
// [Legal]
// Copyright 2020 Esri.

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

        PortalItem {
            id: portalItem
            itemId: "32798dfad17942858d5eef82ee802f0b"
            Component.onCompleted: load();
            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded) {
                    return;
                }

                // if the portal item is a feature collection, add the feature collection to the map's operational layers
                if (type === Enums.PortalItemTypeFeatureCollection) {
                    const featureCollection = ArcGISRuntimeEnvironment.createObject("FeatureCollection", {item: portalItem});
                    const featureCollectionLayer = ArcGISRuntimeEnvironment.createObject("FeatureCollectionLayer", {featureCollection: featureCollection});
                    map.operationalLayers.append(featureCollectionLayer);
                } else {
                    console.warn("Portal item with ID '" + itemId + "' is not a feature collection.")
                }
            }
        }

        Map {
            id: map
            BasemapOceans {}
        }
    }
}
