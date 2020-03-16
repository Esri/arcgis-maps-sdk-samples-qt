// [WriteFile Name=ShowLabelsOnLayers, Category=DisplayInformation]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "ShowLabelsOnLayers.h"

#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "LabelDefinition.h"

using namespace Esri::ArcGISRuntime;

ShowLabelsOnLayers::ShowLabelsOnLayers(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void ShowLabelsOnLayers::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ShowLabelsOnLayers>("Esri.Samples", 1, 0, "ShowLabelsOnLayersSample");
}

void ShowLabelsOnLayers::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the light gray basemap
  m_map = new Map(Basemap::lightGrayCanvas(this), this);

  // Create a feature layer
  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("https://services.arcgis.com/P3ePLMYs2RVChkJx/arcgis/rest/services/USA_115th_Congressional_Districts/FeatureServer/0"), this);
  FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);
  connect(featureLayer, &FeatureLayer::doneLoading, [this, featureLayer](Error e)
  {
    if (!e.isEmpty())
      return;

    m_mapView->setViewpoint(Viewpoint(featureLayer->fullExtent().center(), 56759600));
  });
  m_map->operationalLayers()->append(featureLayer);

  // Apply labels to the feature layer
  const QString republicanJson = createRepublicanJson();
  const QString democratJson = createDemocratJson();
  LabelDefinition* republicanLabelDef = LabelDefinition::fromJson(republicanJson, this);
  LabelDefinition* democratLabelDef = LabelDefinition::fromJson(democratJson, this);
  featureLayer->labelDefinitions()->append(republicanLabelDef);
  featureLayer->labelDefinitions()->append(democratLabelDef);
  featureLayer->setLabelsEnabled(true);

  // Set map to map view
  m_mapView->setViewpointCenter(Point(-10846309.950860, 4683272.219411, SpatialReference::webMercator()), 20000000);
  m_mapView->setMap(m_map);
}

// Creates the label JSON for use in the LabelDefinition
QString ShowLabelsOnLayers::createRepublicanJson() const
{
  // Help regarding the JSON syntax for defining the LabelDefinition.FromJson syntax can be found here:
  // https://developers.arcgis.com/web-map-specification/objects/labelingInfo/
  // This particular JSON string will have the following characteristics:
  // (1) The 'labelExpressionInfo' defines that the label text displayed comes from the fields 'NAME',
  //     'PARTY' (R or D), and 'CDFIPS' in the feature service in the format
  //     "Firstname Lastname (R)
  //     District #".
  // (2) The 'labelPlacement' will be placed horizontally in the polygon.
  // (3) The 'where' clause restricts the labels to data from Republican districts.
  // (4) The 'symbol' for the labeled text will be red with a white halo.

  QString republicanJson = R"rawstring(
                 {
                    "labelExpressionInfo": {
                      "expression": "$feature.NAME + ' (' + left($feature.PARTY,1) + ')\\nDistrict ' + $feature.CDFIPS" // arcade expression
                    },
                    "labelPlacement": "esriServerPolygonPlacementAlwaysHorizontal",
                    "where": "PARTY = 'Republican'",
                    "symbol": {
                      "angle": 0,
                      "backgroundColor": [0,0,0,0],
                      "borderLineColor": [0,0,0,0],
                      "borderLineSize": 0,
                      "color": [255,0,0,255],
                      "font": {
                        "decoration": "none",
                        "size": 8,
                        "style": "normal",
                        "weight": "normal"
                      },
                    "haloColor": [255,255,255,255],
                    "haloSize": 1.5,
                    "horizontalAlignment": "center",
                    "kerning": false,
                    "type": "esriTS",
                    "verticalAlignment": "middle",
                    "xoffset": 0,
                    "yoffset": 0
                  }
                })rawstring";
  return republicanJson;
}

QString ShowLabelsOnLayers::createDemocratJson() const
{
  // This particular JSON string will have the following characteristics:
  // (1) The 'labelExpressionInfo' defines that the label text displayed comes from the fields 'NAME',
  //     'PARTY', and 'CDFIPS' in the feature service in the format
  //     "Firstname Lastname (D)
  //     District #".
  // (2) The 'labelPlacement' will be placed horizontally in the polygon.
  // (3) The 'where' clause restricts the labels to data from Democrat districts.
  // (4) The 'symbol' for the labeled text will be blue with a white halo.

  QString democratJson = R"rawstring(
                           {
                    "labelExpressionInfo": {
                      "expression": "$feature.NAME + ' (' + left($feature.PARTY,1) + ')\\nDistrict ' + $feature.CDFIPS" // arcade expression
                    },
                    "labelPlacement": "esriServerPolygonPlacementAlwaysHorizontal",
                    "where": "PARTY = 'Democrat'",
                    "symbol": {
                      "angle": 0,
                      "backgroundColor": [0,0,0,0],
                      "borderLineColor": [0,0,0,0],
                      "borderLineSize": 0,
                      "color": [0,0,255,255],
                      "font": {
                        "decoration": "none",
                        "size": 8,
                        "style": "normal",
                        "weight": "normal"
                      },
                    "haloColor": [255,255,255,255],
                    "haloSize": 1.5,
                    "horizontalAlignment": "center",
                    "kerning": false,
                    "type": "esriTS",
                    "verticalAlignment": "middle",
                    "xoffset": 0,
                    "yoffset": 0
                  }
                })rawstring";

  return democratJson;
}
