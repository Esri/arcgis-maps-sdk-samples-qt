// [WriteFile Name=ShowDeviceLocationUsingIndoorPositioning, Category=Maps]
// [Legal]
// Copyright 2022 Esri.

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

#include "ShowDeviceLocationUsingIndoorPositioning.h"

#include "FeatureLayer.h"
#include "IndoorsLocationDataSource.h"
#include "Map.h"
#include "MapQuickView.h"

#include "IndoorsLocationDataSourceCreator.h"

using namespace Esri::ArcGISRuntime;

namespace {
  const QString username = "tester_viennardc";
  const QString password = "password.testing12345";

  const QUrl portalUrl = QUrl("https://viennardc.maps.arcgis.com");
  const QString itemId = "89f88764c29b48218366855d7717d266";

  const QString positioningTableName = "ips_positioning";
  const QString pathwaysLayerName = "Pathways";
  const QStringList globalIdSortNames = {"DateCreated", "DATE_CREATED"};

  const QStringList layerNames = {"Details", "Units", "Levels"};
}

ShowDeviceLocationUsingIndoorPositioning::ShowDeviceLocationUsingIndoorPositioning(QObject* parent /* = nullptr */):
  QObject(parent)
{
  Credential* cred = new Credential(username, password, this);
  Portal* portal = new Portal(portalUrl, cred, this);
  m_map = new Map(new PortalItem(portal, itemId, this), this);
}

ShowDeviceLocationUsingIndoorPositioning::~ShowDeviceLocationUsingIndoorPositioning() = default;

void ShowDeviceLocationUsingIndoorPositioning::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ShowDeviceLocationUsingIndoorPositioning>("Esri.Samples", 1, 0, "ShowDeviceLocationUsingIndoorPositioningSample");
}

MapQuickView* ShowDeviceLocationUsingIndoorPositioning::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ShowDeviceLocationUsingIndoorPositioning::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  setupIndoorsLocationDataSource();

  emit mapViewChanged();
}

void ShowDeviceLocationUsingIndoorPositioning::setupIndoorsLocationDataSource()
{
  IndoorsLocationDataSourceCreator* indoorsLocationDataSourceCreator = new IndoorsLocationDataSourceCreator(this);
  connect(indoorsLocationDataSourceCreator, &IndoorsLocationDataSourceCreator::createIndoorsLocationDataSourceCompleted, this, [this](IndoorsLocationDataSource* indoorsLDS)
  {
    qDebug() << "indoors location data source created";
    m_mapView->locationDisplay()->setDataSource(indoorsLDS);
    m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Navigation);
    m_mapView->locationDisplay()->start();

    connect(m_mapView->locationDisplay(), &LocationDisplay::locationChanged, this, &ShowDeviceLocationUsingIndoorPositioning::locationChangedHandler);
    connect(indoorsLDS, &IndoorsLocationDataSource::errorOccurred, this, [](Error e)
    {
      qDebug() << "IndoorsLocationDataSource::errorOccurred" << e.message() << e.additionalMessage();
    });
  });

  indoorsLocationDataSourceCreator->createIndoorsLocationDataSource(m_map, positioningTableName, pathwaysLayerName, globalIdSortNames);
}

void ShowDeviceLocationUsingIndoorPositioning::locationChangedHandler(Location loc)
{
  if (m_locationProperties["floor"] != m_currentFloor)
  {
    m_currentFloor = m_locationProperties["floor"].toInt();
    changeFloorDisplay();
  }
  m_locationProperties = loc.additionalSourceProperties();
  m_locationProperties.insert("horizontalAccuracy", loc.horizontalAccuracy());
  qDebug() << m_locationProperties;

  emit locationChanged();
}

void ShowDeviceLocationUsingIndoorPositioning::changeFloorDisplay()
{
  const LayerListModel* layers = m_map->operationalLayers();
  for (Layer* layer : *layers)
  {
    if (layerNames.contains(layer->name()))
    {
      if (FeatureLayer* featureLayer = dynamic_cast<FeatureLayer*>(layer))
      {
        featureLayer->definitionExpression() = QString{"VERTICAL_ORDER = %1"}.arg(m_currentFloor);
      }
    }
  }
}

QVariantMap ShowDeviceLocationUsingIndoorPositioning::locationProperties() const
{
  return m_locationProperties;
}
