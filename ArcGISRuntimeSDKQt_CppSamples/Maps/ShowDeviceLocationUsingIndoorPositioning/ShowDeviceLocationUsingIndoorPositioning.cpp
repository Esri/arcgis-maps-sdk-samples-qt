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

using namespace Esri::ArcGISRuntime;

ShowDeviceLocationUsingIndoorPositioning::ShowDeviceLocationUsingIndoorPositioning(QObject* parent /* = nullptr */):
  QObject(parent)
{
  Credential* cred = new Credential("tester_viennardc", "password.testing12345", this);
  Portal* portal = new Portal(QUrl("https://viennardc.maps.arcgis.com"), cred, this);
  m_map = new Map(new PortalItem(portal, "89f88764c29b48218366855d7717d266", this), this);
  connect(m_map, &Map::doneLoading, this, &ShowDeviceLocationUsingIndoorPositioning::findPositioningTable);
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

  emit mapViewChanged();
}

void ShowDeviceLocationUsingIndoorPositioning::findPositioningTable()
{
  FeatureTableListModel* tables = m_map->tables();

  for (FeatureTable* table : *tables)
  {
    connect(table, &FeatureTable::doneLoading, this, [table, this]()
    {
      if (table->tableName() == "ips_positioning")
      {
        m_positioningTable = table;
        findPathwaysTable();
      }
    });
    table->load();
  }
}

void ShowDeviceLocationUsingIndoorPositioning::findPathwaysTable()
{
  LayerListModel* layers = m_map->operationalLayers();
  for (Layer* layer : *layers)
  {
    if(FeatureLayer* featureLayer = dynamic_cast<FeatureLayer*>(layer))
    {
      if (featureLayer->name() == "Pathways")
      {
        m_pathwaysTable = dynamic_cast<ArcGISFeatureTable*>(featureLayer->featureTable());
        findGlobalID();
      }
    }
  }
}

void ShowDeviceLocationUsingIndoorPositioning::findGlobalID()
{
  const QList<Field> fields = m_positioningTable->fields();
  Field dateCreatedField;

  for (const Field &field : fields)
  {
    if (field.name().contains("DateCreated", Qt::CaseSensitivity::CaseInsensitive) || field.name().contains("DATE_CREATED", Qt::CaseInsensitive))
    {
      dateCreatedField = field;
    }
  }

  if (dateCreatedField.isEmpty())
  {
    qWarning() << "Date created field not found";
    return;
  }

  QueryParameters queryParameters;
  queryParameters.setMaxFeatures(1);
  queryParameters.setWhereClause("1=1");
  queryParameters.orderByFields().append(OrderBy(dateCreatedField.name(), SortOrder::Descending));

  connect(m_positioningTable, &FeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* rawFeatureQueryResult)
  {
    // Delete rawFeatureQueryResult pointer when we leave scope.
    auto featureQueryResult = std::unique_ptr<FeatureQueryResult>(rawFeatureQueryResult);

    FeatureIterator iter = featureQueryResult->iterator();
    if (iter.hasNext())
    {
      Feature* feat = iter.next();
      QUuid globalID = feat->attributes()->attributesMap().value("GlobalID").toUuid();
      setupIndoorLocationDataSource(globalID);
    }
  });
  m_positioningTable->queryFeatures(queryParameters);
}

void ShowDeviceLocationUsingIndoorPositioning::setupIndoorLocationDataSource(QUuid globalID)
{
  IndoorsLocationDataSource* indoorsLocationDataSource = new IndoorsLocationDataSource(m_positioningTable, m_pathwaysTable, globalID, this);
  IndoorsLocationDataSource* indoorsLocationDataSource2 = new IndoorsLocationDataSource(m_positioningTable, m_pathwaysTable, this);
  qDebug() << indoorsLocationDataSource->positioningId();
  qDebug() << indoorsLocationDataSource2->positioningId();


  m_mapView->locationDisplay()->setDataSource(indoorsLocationDataSource);
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Navigation);
  m_mapView->locationDisplay()->start();
  connect(m_mapView->locationDisplay(), &LocationDisplay::locationChanged, this, &ShowDeviceLocationUsingIndoorPositioning::locationChanged);
}

void ShowDeviceLocationUsingIndoorPositioning::locationChanged(Location loc)
{
  qDebug() << loc.additionalSourceProperties();
}
