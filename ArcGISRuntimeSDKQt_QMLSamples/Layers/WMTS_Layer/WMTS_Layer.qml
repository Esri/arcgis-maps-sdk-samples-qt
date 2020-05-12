// [WriteFile Name=WMTS_Layer, Category=Layers]
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
import Esri.ArcGISRuntime 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url wmtsServiceUrl: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/WorldTimeZones/MapServer/WMTS"
    property WmtsService service;

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: createWmtsLayer();
    }

    function createWmtsLayer() {
        // create the service
        service = ArcGISRuntimeEnvironment.createObject("WmtsService", { url: wmtsServiceUrl });

        // connect to loadStatusChanged signal of the service
        service.loadStatusChanged.connect(()=> {
            if (service.loadStatus === Enums.LoadStatusLoaded) {
                // get the layer info list
                const serviceInfo = service.serviceInfo;
                const layerInfos = serviceInfo.layerInfos;
                // get the first layer id from the list
                const layerId = layerInfos[0].wmtsLayerId;
                // create WMTS layer
                const wmtsLayer = ArcGISRuntimeEnvironment.createObject("WmtsLayer", {
                                                                            url: wmtsServiceUrl,
                                                                            layerId: layerId
                                                                        });
                // create a basemap from the layer
                const basemap = ArcGISRuntimeEnvironment.createObject("Basemap");
                basemap.baseLayers.append(wmtsLayer);
                // create a map
                const map = ArcGISRuntimeEnvironment.createObject("Map", {
                                                                      basemap: basemap
                                                                  });
                // set the map on the mapview
                mapView.map = map;
            }
        });

        // load the service
        service.load();
    }
}
