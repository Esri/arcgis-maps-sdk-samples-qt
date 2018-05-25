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
  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/USA/MapServer/1"), this);
  FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);
  connect(featureLayer, &FeatureLayer::doneLoading, [this, featureLayer](Error e)
  {
    if (!e.isEmpty())
      return;

    m_mapView->setViewpoint(Viewpoint(featureLayer->fullExtent().center(), 56759600));
  });
  m_map->operationalLayers()->append(featureLayer);

  // Apply labels to the feature layer
  const QString labelJson = createLabelJson();
  LabelDefinition* labelDef = LabelDefinition::fromJson(labelJson, this);
  featureLayer->labelDefinitions()->append(labelDef);
  featureLayer->setLabelsEnabled(true);

  // Set map to map view
  m_mapView->setMap(m_map);
}

// Creates the label JSON for use in the LabelDefinition
QString ShowLabelsOnLayers::createLabelJson() const
{
  // Help regarding the JSON syntax for defining the LabelDefinition.FromJson syntax can be found here:
  // https://developers.arcgis.com/web-map-specification/objects/labelingInfo/
  // This particular JSON string will have the following characteristics:
  // (1) The 'labelExpressionInfo' defines that the label text displayed comes from the field 'rte_num1' in the
  //     feature service and will be prefaced with an "I -". Example: "I - 10", "I - 15", "I - 95", etc.
  // (2) The 'labelPlacement' will be placed above and along the highway polyline segment.
  // (3) The 'where' clause restricts the labels to be displayed that has valid (non-empty) data. Empty data
  //     for this service has a single blank space in the 'rte_num1' field.
  // (4) The 'symbol' for the labeled text will be blue with a yellow halo.
  return QString(QStringLiteral("{\"labelExpressionInfo\":{\"expression\":\"'I - ' + $feature.rte_num1\"},\"labelPlacement\":\"esriServerLinePlacementAboveAlong\",\"where\":\"rte_num1 <> ' '\",\"symbol\": {\"angle\":0,\"backgroundColor\":[0,0,0,0],\"borderLineColor\":[0,0,0,0],\"borderLineSize\":0,\"color\":[0,0,255,255],\"font\": {\"decoration\":\"none\",\"size\":15,\"style\":\"normal\",\"weight\":\"normal\"},\"haloColor\":[255,255,0,255],\"haloSize\":1.5,\"horizontalAlignment\":\"center\",\"kerning\":false,\"type\":\"esriTS\",\"verticalAlignment\":\"middle\",\"xoffset\":0,\"yoffset\":0}}"));
}
