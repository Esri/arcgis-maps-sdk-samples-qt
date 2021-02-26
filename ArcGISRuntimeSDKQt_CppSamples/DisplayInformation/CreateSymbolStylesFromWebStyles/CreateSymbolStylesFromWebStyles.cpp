// [WriteFile Name=CreateSymbolStylesFromWebStyles, Category=DisplayInformation]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "CreateSymbolStylesFromWebStyles.h"

#include "FeatureLayer.h"
#include "ServiceFeatureTable.h"
#include "Map.h"
#include "MapQuickView.h"

#include "UniqueValueRenderer.h"
#include "SymbolStyle.h"

using namespace Esri::ArcGISRuntime;

CreateSymbolStylesFromWebStyles::CreateSymbolStylesFromWebStyles(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISNavigation, this))
{
  m_map->setReferenceScale(100'000);

  QUrl webStyleLayerUrl = QUrl("http://services.arcgis.com/V6ZHFr6zdgNZuVG0/arcgis/rest/services/LA_County_Points_of_Interest/FeatureServer/0");
  FeatureLayer* webStyleLayer = new FeatureLayer(new ServiceFeatureTable(webStyleLayerUrl, this), this);
  m_map->operationalLayers()->append(webStyleLayer);

  UniqueValueRenderer* uniqueValueRenderer = new UniqueValueRenderer(this);
  uniqueValueRenderer->fieldNames().append("cat2");
  webStyleLayer->setRenderer(uniqueValueRenderer);

  SymbolStyle* symbolStyle = new SymbolStyle("Esri2DPointSymbolsStyle", this);

  QList<QString> symbolNames = {"atm", "beach", "campground", "city-hall", "hospital", "library", "park", "place-of-worship", "police-station", "post-office", "school", "trail"};

  for (const QString &symbolName : symbolNames)
  {
    connect(symbolStyle, &SymbolStyle::fetchSymbolCompleted, this, [this](QUuid taskId, Symbol* symbol)
    {
      // QList<QString> categories =
    });

    // symbolStyle->fetchSymbol({symbolName});
  }
}

CreateSymbolStylesFromWebStyles::~CreateSymbolStylesFromWebStyles() = default;

void CreateSymbolStylesFromWebStyles::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<CreateSymbolStylesFromWebStyles>("Esri.Samples", 1, 0, "CreateSymbolStylesFromWebStylesSample");
}

MapQuickView* CreateSymbolStylesFromWebStyles::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void CreateSymbolStylesFromWebStyles::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  const double x_longitude = -118.44186;
  const double y_latitude = 34.38301;
  const Point centerPt(x_longitude, y_latitude, SpatialReference::wgs84());
  const double scale = 7000;

  m_mapView->setViewpointCenter(centerPt, scale);

  emit mapViewChanged();
}

QList<QString> CreateSymbolStylesFromWebStyles::mapSymbolNameToField(QString symbolName)
{
  QMap<QString,QList<QString>> categories = {};
}
