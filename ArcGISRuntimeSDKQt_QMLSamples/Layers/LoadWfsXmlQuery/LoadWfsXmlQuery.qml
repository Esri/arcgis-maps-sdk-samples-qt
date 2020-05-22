// [WriteFile Name=LoadWfsXmlQuery, Category=Layers]
// [Legal]
// Copyright 2019 Esri.

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

    readonly property string xmlQueryString :
     '
     <wfs:GetFeature service="WFS" version="2.0.0"
     xmlns:Seattle_Downtown_Features="https://dservices2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/services/Seattle_Downtown_Features/WFSServer"
     xmlns:wfs="http://www.opengis.net/wfs/2.0"
     xmlns:fes="http://www.opengis.net/fes/2.0"
     xmlns:gml="http://www.opengis.net/gml/3.2">
     <wfs:Query typeNames="Seattle_Downtown_Features:Trees">
     <fes:Filter>
     <fes:PropertyIsLike wildCard="*" escapeChar="\">
     <fes:ValueReference>Trees:SCIENTIFIC</fes:ValueReference>
     <fes:Literal>Tilia *</fes:Literal>
     </fes:PropertyIsLike>
     </fes:Filter>
     </wfs:Query>
     </wfs:GetFeature>
     '

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapNavigationVector {}

            FeatureLayer {
                WfsFeatureTable {
                    id: wfsFeatureTable
                    url: "https://dservices2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/services/Seattle_Downtown_Features/WFSServer?service=wfs&request=getcapabilities"
                    tableName: "Seattle_Downtown_Features:Trees"
                    featureRequestMode: Enums.FeatureRequestModeManualCache
                    onLoadStatusChanged: {
                        if (loadStatus !== Enums.LoadStatusLoaded)
                            return;

                        // populate the table with XML Query
                        populateFromServiceWithXml(xmlQueryString, true);

                        // set the viewpoint to the center of the extent
                        mapView.setViewpointGeometryAndPadding(extent, 50);
                    }
                }
            }
        }
    }
}
