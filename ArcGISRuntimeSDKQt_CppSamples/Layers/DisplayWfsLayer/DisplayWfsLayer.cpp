// [WriteFile Name=DisplayWfsLayer, Category=Layers]
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

#include "DisplayWfsLayer.h"

#include "Map.h"
#include "MapQuickView.h"
#include "WfsFeatureTable.h"
#include "FeatureLayer.h"
#include "QueryParameters.h"
#include "SimpleRenderer.h"
#include "SimpleLineSymbol.h"

using namespace Esri::ArcGISRuntime;

DisplayWfsLayer::DisplayWfsLayer(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::topographic(this), this))
{
  // create WFS Feature Table
  m_wfsFeatureTable = new WfsFeatureTable(QUrl("https://dservices2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/services/Seattle_Downtown_Features/WFSServer?service=wfs&request=getcapabilities"),
                                          "Seattle_Downtown_Features:Buildings", this);

  // Set feature request mode to manual - only manual is supported at v100.5.
  // In this mode, you must manually populate the table - panning and zooming
  // won't request features automatically.
  m_wfsFeatureTable->setFeatureRequestMode(FeatureRequestMode::ManualCache);

  // populate the feature table once loaded
  connect(m_wfsFeatureTable, &WfsFeatureTable::doneLoading, this, [this](Error e)
  {
    if (e.isEmpty())
      populateWfsFeatureTable();
  });

  // create feature layer from the feature table
  FeatureLayer* featureLayer = new FeatureLayer(m_wfsFeatureTable, this);

  // add a renderer to the layer
  SimpleLineSymbol* sls = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("red"), 3.0f, this);
  SimpleRenderer* sr = new SimpleRenderer(sls, this);
  featureLayer->setRenderer(sr);

  // add the layer to the map
  m_map->operationalLayers()->append(featureLayer);

  // set initial viewpoint
  Envelope env(-122.341581, 47.613758, -122.332662, 47.617207, SpatialReference::wgs84());
  m_map->setInitialViewpoint(Viewpoint(env));
}

DisplayWfsLayer::~DisplayWfsLayer() = default;

void DisplayWfsLayer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayWfsLayer>("Esri.Samples", 1, 0, "DisplayWfsLayerSample");
}

MapQuickView* DisplayWfsLayer::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void DisplayWfsLayer::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  connect(m_mapView, &MapQuickView::navigatingChanged, this, [this]()
  {
    if (m_mapView->isNavigating())
      return;

    populateWfsFeatureTable();
  });

  emit mapViewChanged();
}

void DisplayWfsLayer::populateWfsFeatureTable()
{
  if (!m_mapView || !m_wfsFeatureTable)
    return;

  // create query parameters
  QueryParameters params;
  params.setGeometry(m_mapView->visibleArea().extent());
  params.setSpatialRelationship(SpatialRelationship::Intersects);

  // query the service
  constexpr bool clearCache = false;
  const QStringList outFields = {"*"};
  m_wfsFeatureTable->populateFromService(params, clearCache, outFields);
}
