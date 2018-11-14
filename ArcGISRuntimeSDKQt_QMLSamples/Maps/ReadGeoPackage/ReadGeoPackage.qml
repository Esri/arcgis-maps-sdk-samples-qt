
// [WriteFile Name=ReadGeoPackage, Category=Maps]
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
import Esri.ArcGISRuntime 100.5
import QtQuick.Controls 2.2
import Esri.ArcGISExtras 1.1

Rectangle {
  id: rootRectangle
  clip: true
  width: 800
  height: 600
  
  property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/gpkg/"

  MapView {
    id: mapView
    anchors.fill: parent

    Map {
      id: map
      initialViewpoint: vc
      BasemapTopographic {}
    }

    ViewpointCenter {
      id: vc
      center: Point {
        x: -104.8319
        y: 39.7294
        spatialReference: SpatialReference {
          wkid: 4326
        }
      }

      targetScale: 500000
    }
  }

  // Layer selection menu (interaction area)
  Rectangle {
    id: layerVisibilityRect
    anchors {
      margins: 10
      left: parent.left
      top: parent.top
    }

    height: 210
    width: 225
    color: "transparent"

    MouseArea {
      anchors.fill: parent
      onClicked: mouse.accepted = true
      onWheel: wheel.accepted = true
    }

    // Layer selectiom menu (data population + UI)
    Rectangle {
      anchors.fill: parent
      width: layerVisibilityRect.width
      height: layerVisibilityRect.height
      color: "lightgrey"
      opacity: 0.9
      radius: 5
      border {
        color: "#4D4D4D"
        width: 1
      }

      Column {
        anchors {
          fill: parent
          margins: 10
        }

        clip: true

        Text {
          width: parent.width
          text: "GeoPackage Layers"
          wrapMode: Text.WordWrap
          clip:true
          font {
            pixelSize: 14
            bold: true
          }
        }

        // Load and read the GeoPackage
        GeoPackage {
          id: gpkg
          path: dataPath + "AuroraCO.gpkg"

          // Initial check to see if GeoPackage is loaded and not empty
          onLoadStatusChanged: {
            if (loadStatus !== Enums.LoadStatusLoaded)
              return;

            if (!gpkg.geoPackageRasters.length > 0 && !gpkg.geoPackageFeatureTables.length > 0)
              return;

            // For each layer, create an ArcGISRuntimeEnvironment layer object using its data. Append this to a listModel; see below. (Do this for both rasterLayers and featureTables)
            for (var i = 0; i < gpkg.geoPackageFeatureTables.length; i++){
              var featureLayer = ArcGISRuntimeEnvironment.createObject("FeatureLayer", {featureTable: gpkg.geoPackageFeatureTables[i]}, gpkg);
              layerList.append({lyr: featureLayer, name: featureLayer.name});
              featureLayer.load();
            }

            for(var k = 0; k < gpkg.geoPackageRasters.length; k++){
              var raster = gpkg.geoPackageRasters[k];
              var rasterLayer = ArcGISRuntimeEnvironment.createObject("RasterLayer", {raster: raster}, gpkg);
              var rasterName = String(raster.path).substring(String(raster.path).lastIndexOf("/") + 1);
              layerList.append({lyr: rasterLayer, name: rasterName});
              rasterLayer.load();
            }
          }
        }

        // To be used as a container for the layers
        ListModel {
          id: layerList
        }

        // This will populate the menu by using a delegate to list through the previous listModel with the layers in it. For each layer in the listModel, a row is added to the menu with an
        // accompanying label and toggleß switch.
        ListView {
          id: layerVisibilityListView
          anchors.margins: 20
          width: parent.width
          height: parent.height
          clip: true
          model: layerList
          delegate: Item {
            id: layerVisibilityDelegate
            width: parent.width
            height: 35

            Row {
              spacing: 5
              anchors.verticalCenter: parent.verticalCenter
              Text {
                width: 150
                text: name
                wrapMode: Text.WordWrap
                font.pixelSize: 14
              }

              Switch {
                // Fires whenever the switch is toggled. If checked, add layer to the map (if not already within the oprational layers) and make visible. If unchecked, hide the layer.
                onCheckedChanged: {
                  if (checked) {
                    if(!map.operationalLayers.contains(lyr)) {
                      map.operationalLayers.append(lyr);
                    }
                    lyr.visible = true;
                  } else {
                    lyr.visible = false;
                  }
                }
              }
            }
          }
        }

        // Load the GeoPackage on app start
        Component.onCompleted: {
          gpkg.load();
        }
      }
    }
  }
}
