// [WriteFile Name=DisplaySubtypeFeatureLayer, Category=Layers]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "DisplaySubtypeFeatureLayer.h"

#include "FeatureLayer.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "SubtypeFeatureLayer.h"
#include "SubtypeSublayer.h"
#include "SubtypeSublayerListModel.h"
#include "ServiceFeatureTable.h"
#include "Map.h"
#include "MapQuickView.h"

using namespace Esri::ArcGISRuntime;

DisplaySubtypeFeatureLayer::DisplaySubtypeFeatureLayer(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::streetsNightVector(this), this)),
  m_alternateRenderer(new SimpleRenderer(new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, QColor(Qt::GlobalColor::magenta), 20, this), this))
{
  // create the feature table
  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer/100"), this);
  // create the feature layer using the feature table
  m_subtypeFeatureLayer = new SubtypeFeatureLayer(featureTable, this);

  // add the feature layer to the map
  m_map->operationalLayers()->append(m_subtypeFeatureLayer);

  // set the viewpoint to Naperville, Illinois
  m_map->setInitialViewpoint(Viewpoint(Envelope(-9812691.11079696, 5128687.20710657, -9812377.9447607, 5128865.36767282, SpatialReference::webMercator())));

  connect(m_subtypeFeatureLayer, &SubtypeFeatureLayer::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
      return;

    m_busy = false;
    emit busyChanged();

    m_subtypeSublayer = m_subtypeFeatureLayer->sublayerWithSubtypeName("Street Light", this);
    m_labelDefinition = LabelDefinition::fromJson(m_labelJson, this);

    if (!m_labelDefinition)
    {
      qDebug() << "m_labelDefinition";
      return;
    }

    if (!m_subtypeSublayer)
      return;

    m_subtypeSublayer->labelDefinitions()->append(m_labelDefinition);
    m_originalRenderer = m_subtypeSublayer->renderer()->clone(this);
    m_subtypeSublayer->setLabelsEnabled(true);
    m_subtypeSublayer->setMinScale(3000.0);
    m_sublayerMinScale = m_subtypeSublayer->minScale();
    emit sublayerMinScaleChanged();



  });

  connect(m_map, &Map::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
      return;

    connect(m_mapView, &MapQuickView::mapScaleChanged, this, [this]()
    {
      m_mapScale = m_mapView->mapScale();
      emit mapScaleChanged();
    });
  });
}

DisplaySubtypeFeatureLayer::~DisplaySubtypeFeatureLayer() = default;

void DisplaySubtypeFeatureLayer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplaySubtypeFeatureLayer>("Esri.Samples", 1, 0, "DisplaySubtypeFeatureLayerSample");
}

MapQuickView* DisplaySubtypeFeatureLayer::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void DisplaySubtypeFeatureLayer::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}

 void DisplaySubtypeFeatureLayer::switchSublayerVisibility()
 {
   qDebug() << "switchSublayerVisibility";
   if (m_subtypeSublayer)
     m_subtypeSublayer->setVisible(!m_subtypeSublayer->isVisible());
 }

 void DisplaySubtypeFeatureLayer::setOringalRenderer()
 {
   qDebug() << "setOringalRenderer";
   if (m_subtypeSublayer)
    m_subtypeSublayer->setRenderer(m_originalRenderer);
 }

 void DisplaySubtypeFeatureLayer::setAlternativeRenderer()
 {
   qDebug() << "setAlternativeRenderer";
   if (m_subtypeSublayer)
    m_subtypeSublayer->setRenderer(m_alternateRenderer);
 }

 void DisplaySubtypeFeatureLayer::setSublayerMinScale()
 {
   qDebug() << "setSublayerMinScale";
   m_subtypeSublayer->setMinScale(m_mapView->mapScale());
   m_sublayerMinScale = m_subtypeSublayer->minScale();
   emit sublayerMinScaleChanged();
 }
