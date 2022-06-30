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

#include "ArcGISFeatureTable.h"
#include "FeatureLayer.h"
#include "FeatureTable.h"
#include "IndoorsLocationDataSource.h"
#include "Location.h"
#include "Map.h"
#include "MapQuickView.h"
#include "OrderBy.h"
#include "Portal.h"
#include "PortalItem.h"

#include "IndoorsLocationDataSourceCreator.h"

using namespace Esri::ArcGISRuntime;

ShowDeviceLocationUsingIndoorPositioning::ShowDeviceLocationUsingIndoorPositioning(QObject* parent /* = nullptr */):
  QObject(parent)
{
  Credential* cred = new Credential("tester_viennardc", "password.testing12345", this);
  Portal* portal = new Portal(QUrl("https://viennardc.maps.arcgis.com"), cred, this);
  m_map = new Map(new PortalItem(portal, "89f88764c29b48218366855d7717d266", this), this);
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

  qDebug() << m_mapView->locationDisplay()->isStarted();

  emit mapViewChanged();
}

void ShowDeviceLocationUsingIndoorPositioning::setupIndoorsLocationDataSource()
{
  auto indoorsLocationDataSourceCreator = new IndoorsLocationDataSourceCreator(this);
  connect(indoorsLocationDataSourceCreator, &IndoorsLocationDataSourceCreator::createIndoorsLocationDataSourceCompleted, this, [this](IndoorsLocationDataSource* indoorsLDS)
  {
    qDebug() << "indoors location data source created";
    m_mapView->locationDisplay()->setDataSource(indoorsLDS);
    m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Navigation);
    m_mapView->locationDisplay()->start();
    connect(m_mapView->locationDisplay(), &LocationDisplay::locationChanged, this, &ShowDeviceLocationUsingIndoorPositioning::locationChanged);
    connect(indoorsLDS, &IndoorsLocationDataSource::errorOccurred, this, [](Error e)
    {
      qDebug() << "IndoorsLocationDataSource::errorOccurred" << e.message() << e.additionalMessage();
    });

    connect(indoorsLDS, &IndoorsLocationDataSource::statusChanged, this, [indoorsLDS](bool started)
    {
      qDebug() << "IndoorsLocationDataSource::statusChanged" << "started" << started << "isStarted" << indoorsLDS->isStarted();
    });
  });

  indoorsLocationDataSourceCreator->createIndoorsLocationDataSource(m_map, "ips_positioning", "Pathways", {"DateCreated", "DATE_CREATED"});
}

void ShowDeviceLocationUsingIndoorPositioning::locationChanged(Location loc)
{
  qDebug() << loc.additionalSourceProperties();
  qDebug() << "horizontal accuracy" << loc.horizontalAccuracy();
}
