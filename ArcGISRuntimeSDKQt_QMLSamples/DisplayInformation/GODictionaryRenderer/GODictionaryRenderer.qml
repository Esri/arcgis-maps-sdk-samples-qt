// [WriteFile Name=GODictionaryRenderer, Category=DisplayInformation]
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

import QtQuick
import QtQuick.Controls
<<<<<<< HEAD
import QtQuick.XmlListModel
import Esri.ArcGISRuntime
import Esri.ArcGISExtras
=======
import Esri.ArcGISRuntime
import Esri.ArcGISExtras
import Esri.samples
>>>>>>> v.next

Rectangle {
    width: 800
    height: 600
<<<<<<< HEAD

    readonly property url dataPath: {
        Qt.platform.os === "ios" ?
                    System.writableLocationUrl(System.StandardPathsDocumentsLocation) + "/ArcGIS/Runtime/Data" :
                    System.writableLocationUrl(System.StandardPathsHomeLocation) + "/ArcGIS/Runtime/Data"
    }
    property bool graphicsLoaded: false

    Map {
        id: map
        Basemap {
            initStyle: Enums.BasemapStyleArcGISTopographic
        }
=======

    readonly property url dataPath: {
        Qt.platform.os === "ios" ?
                    System.writableLocationUrl(System.StandardPathsDocumentsLocation) + "/ArcGIS/Runtime/Data" :
                    System.writableLocationUrl(System.StandardPathsHomeLocation) + "/ArcGIS/Runtime/Data"
>>>>>>> v.next
    }
    property bool graphicsLoaded: false


    // Create MapView that a GraphicsOverlay
    // for the military symbols.
    MapView {
        id: mapView
        anchors.fill: parent

<<<<<<< HEAD
        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
=======
        Map {
            id: map
            Basemap {
                initStyle: Enums.BasemapStyleArcGISTopographic
            }
        }

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();

            // Read the XML file and create a graphic from each entry
            xmlParser.parseXmlFileAsync(dataPath + "/xml/arcade_style/Mil2525DMessages.xml");
>>>>>>> v.next
        }

        // The GraphicsOverlay does not have a valid extent until it has been added
        // to a MapView with a valid SpatialReference
        onSpatialReferenceChanged: {
            setViewpointGeometryAndPadding( graphicsOverlay.extent, 20 );
        }

        //! [Apply Dictionary Renderer Graphics Overlay QML]
        GraphicsOverlay {
            id: graphicsOverlay

            DictionaryRenderer {
                id: dictionaryRenderer
                dictionarySymbolStyle: Factory.DictionarySymbolStyle.createFromFile(dataPath + "/styles/arcade_style/mil2525d.stylx")

                Component.onCompleted: {
                    dictionarySymbolStyle.loadStatusChanged.connect(() => {
                                                                        if (dictionarySymbolStyle.loadStatus === Enums.LoadStatusLoaded) {
                                                                            const dictionarySymbolStyleConfigurations = dictionarySymbolStyle.configurations;
                                                                            for (let i = 0; i < dictionarySymbolStyleConfigurations.length; i++) {
                                                                                if (dictionarySymbolStyleConfigurations[i].name === "model") {
                                                                                    dictionarySymbolStyleConfigurations[i].value = "ORDERED ANCHOR POINTS";
                                                                                }
                                                                            }
                                                                        }
                                                                    });
                }
            }
        }
        //! [Apply Dictionary Renderer Graphics Overlay QML]
    }

    ProgressBar {
        id: progressBar_loading
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            margins: 5
        }
        indeterminate: true
        visible: !graphicsLoaded
    }

    XmlParser {
        id: xmlParser

        onXmlParseComplete: (parsedXml) => {
                                parsedXml.forEach(element => {createGraphicFromElement(element)});
                                graphicsLoaded = true;
                            }
    }

    function createGraphicFromElement(element) {
        let wkid = element._wkid;
        if (!wkid) {
            // If _wkid was absent, use WGS 1984 (4326) by default.
            wkid = 4326;
        }
        const pointStrings = element._control_points.split(";");
        const sr = ArcGISRuntimeEnvironment.createObject("SpatialReference", { wkid: wkid });
        let geom;
        if (pointStrings.length === 1) {
            // It's a point
            const pointBuilder = ArcGISRuntimeEnvironment.createObject("PointBuilder");
            pointBuilder.spatialReference = sr;
            const coords = pointStrings[0].split(",");
            pointBuilder.setXY(coords[0], coords[1]);
            geom = pointBuilder.geometry;
        } else {
            const builder = ArcGISRuntimeEnvironment.createObject("MultipointBuilder");
            builder.spatialReference = sr;

            for (let ptIndex = 0; ptIndex < pointStrings.length; ptIndex++) {
                const coords = pointStrings[ptIndex].split(",");
                builder.points.addPointXY(coords[0], coords[1]);
            }
            geom = builder.geometry;
        }
        if (geom) {
            const graphic = ArcGISRuntimeEnvironment.createObject("Graphic", { geometry: geom });
            graphic.attributes.attributesJson = {
                "identity": element.identity,
                "symbolset": element.symbolset,
                "symbolentity": element.symbolentity,
                "echelon": element.echelon,
                "specialentitysubtype": element.specialentitysubtype,
                "indicator": element.indicator,
                "modifier2": element.modifier2,
                "uniquedesignation": element.uniquedesignation,
                "additionalinformation": element.additionalinformation
            };
            graphicsOverlay.graphics.append(graphic);
        }
    }
}
