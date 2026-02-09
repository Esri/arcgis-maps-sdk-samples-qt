// [WriteFile Name=QueryDynamicEntities, Category=Search]
// [Legal]
// Copyright 2026 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "CustomDynamicEntityDataSource.h"
#include "QueryDynamicEntities.h"

// ArcGIS Maps SDK headers
#include "AttributeListModel.h"
#include "CoreTypes.h"
#include "DynamicEntity.h"
#include "DynamicEntityChangedInfo.h"
#include "DynamicEntityDataSourcePurgeOptions.h"
#include "DynamicEntityIterator.h"
#include "DynamicEntityLayer.h"
#include "DynamicEntityObservation.h"
#include "DynamicEntityQueryParameters.h"
#include "DynamicEntityQueryResult.h"
#include "Envelope.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LabelDefinition.h"
#include "LabelDefinitionListModel.h"
#include "LabelingTypes.h"
#include "LayerListModel.h"
#include "LinearUnit.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "Polygon.h"
#include "RealTimeTypes.h"
#include "SimpleFillSymbol.h"
#include "SimpleLabelExpression.h"
#include "SimpleLineSymbol.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "TextSymbol.h"
#include "TrackDisplayProperties.h"
#include "Viewpoint.h"

// Qt headers
#include <QFuture>
#include <QStandardPaths>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

    return dataPath;
  }
} // namespace

QueryDynamicEntities::QueryDynamicEntities(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
  m_resultsModel = new FlightInfoListModel(this);
}

QueryDynamicEntities::~QueryDynamicEntities() = default;

void QueryDynamicEntities::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<QueryDynamicEntities>("Esri.Samples", 1, 0, "QueryDynamicEntitiesSample");
}

FlightInfoListModel* QueryDynamicEntities::resultsModel() const
{
  return m_resultsModel;
}

void QueryDynamicEntities::connectEntityUpdates(DynamicEntity* entity, const QString& trackId)
{
  if (!entity || trackId.isEmpty() || !m_resultsModel)
  {
    return;
  }

  // If a connection already exists for this track, disconnect to avoid duplicates
  if (m_entityConnections.contains(trackId))
  {
    QObject::disconnect(m_entityConnections.value(trackId));
    m_entityConnections.remove(trackId);
  }

  QMetaObject::Connection conn = connect(entity, &DynamicEntity::dynamicEntityChanged, this, [this, trackId](DynamicEntityChangedInfo* changedInfo)
  {
    changedInfo->deleteLater();
    DynamicEntityObservation* obs = changedInfo->receivedObservation();
    if (!obs)
    {
      return;
    }
    const QVariantMap updatedMap = obs->attributes()->attributesMap();
    m_resultsModel->setAttributesForTrack(trackId, updatedMap);
    emit resultsModelChanged();
  });
  m_entityConnections.insert(trackId, conn);
}

void QueryDynamicEntities::clearEntityUpdateConnections()
{
  for (QHash<QString, QMetaObject::Connection>::iterator it = m_entityConnections.begin(); it != m_entityConnections.end(); ++it)
  {
    QObject::disconnect(it.value());
  }
  m_entityConnections.clear();
}

void QueryDynamicEntities::addResultEntity(DynamicEntity* result, bool selectEntity)
{
  if (!result || !m_resultsModel)
  {
    return;
  }

  const QVariantMap map = result->attributes()->attributesMap();
  const QString trackId = map.value("flight_number").toString();
  if (trackId.isEmpty())
  {
    return;
  }

  if (selectEntity && m_dynamicEntityLayer)
  {
    m_dynamicEntityLayer->selectDynamicEntity(result);
  }

  m_resultsModel->setAttributesForTrack(trackId, map);
  m_resultEntities.insert(trackId, result);
  connectEntityUpdates(result, trackId);
}

MapQuickView* QueryDynamicEntities::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void QueryDynamicEntities::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->setViewpointAsync(Viewpoint{Point{-112.0101, 33.4352, SpatialReference::wgs84()}, 400000});

  emit mapViewChanged();
  setUpGraphics();
}

void QueryDynamicEntities::setUpGraphics()
{
  // Create a graphics overlay for the airport buffer.
  m_bufferGraphicsOverlay = new GraphicsOverlay();
  m_mapView->graphicsOverlays()->append(m_bufferGraphicsOverlay);

  // Create a 15-mile geodetic buffer around Phoenix Airport (PHX).
  m_phoenixAirportBuffer = new Polygon(GeometryEngine::bufferGeodetic(Point{-112.0101, 33.4352, SpatialReference::wgs84()}, 15,
                                                                      LinearUnit(LinearUnitId::Miles), NAN, GeodeticCurveType::Geodesic));

  // Create a semi-transparent fill symbol for the buffer and add it to the overlay.
  SimpleFillSymbol* bufferSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(255, 0, 0, 75),
                                                        new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("Black"), 1.0, this), this);

  // Add the buffer graphic to the overlay and hide it initially.
  m_bufferGraphicsOverlay->graphics()->append(new Graphic(*m_phoenixAirportBuffer, bufferSymbol));
  m_bufferGraphicsOverlay->setVisible(false);

  setupDynamicEntityDataSource();
}

void QueryDynamicEntities::setupDynamicEntityDataSource()
{
  QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/dynamicEntity/phx_air_traffic.json";
  m_dataSource = new CustomDynamicEntityDataSource(dataPath, "flight_number", 100, this);

  m_dynamicEntityLayer = new DynamicEntityLayer(m_dataSource, this);

  // Ensure the data source begins emitting observations
  QFuture<void> connectFuture = m_dataSource->connectDataSourceAsync();
  Q_UNUSED(connectFuture)

  // Connect the data source and configure once connected
  connect(m_dataSource, &DynamicEntityDataSource::connectionStatusChanged, this, [this](ConnectionStatus status)
  {
    if (status == ConnectionStatus::Connected)
    {
      m_dynamicEntityLayer->dataSource()->purgeOptions()->setMaximumObservationsPerTrack(20);
    }
  });

  // Set previous observation properties
  m_dynamicEntityLayer->trackDisplayProperties()->setShowPreviousObservations(true);
  m_dynamicEntityLayer->trackDisplayProperties()->setShowTrackLine(true);
  m_dynamicEntityLayer->trackDisplayProperties()->setMaximumObservations(20);

  // Label dynamic entity with its flight number above the point.
  SimpleLabelExpression* simpleLabelExpression = new SimpleLabelExpression("[flight_number]", this);
  TextSymbol* labelSymbol = new TextSymbol(this);
  labelSymbol->setColor(Qt::red);
  labelSymbol->setSize(12);
  labelSymbol->setHaloColor(Qt::white);
  labelSymbol->setHaloWidth(2);

  LabelDefinition* labelDef = new LabelDefinition(simpleLabelExpression, labelSymbol, this);
  labelDef->setPlacement(LabelingPlacement::PointAboveCenter);
  m_dynamicEntityLayer->labelDefinitions()->append(labelDef);
  m_dynamicEntityLayer->setLabelsEnabled(true);

  m_map->operationalLayers()->append(m_dynamicEntityLayer);
}

void QueryDynamicEntities::handleQuerySelection(const QString& queryType)
{
  m_dynamicEntityLayer->clearSelection();
  m_bufferGraphicsOverlay->setVisible(false);

  // Disconnect any existing entity update subscriptions before running a new query
  clearEntityUpdateConnections();

  if (queryType == "Within 15 Miles of PHX")
  {
    m_bufferGraphicsOverlay->setVisible(true);

    m_params = new DynamicEntityQueryParameters();
    m_params->setGeometry(*m_phoenixAirportBuffer);
    m_params->setSpatialRelationship(SpatialRelationship::Intersects);

    // Perform query using geometry on the data source and select returned dynamic entities.
    m_dataSource->queryDynamicEntitiesAsync(m_params).then(this, [this](DynamicEntityQueryResult* results)
    {
      if (!results)
      {
        return;
      }
      // Clear previous results
      m_resultsModel->clear();
      m_resultEntities.clear();

      for (DynamicEntity* result : results->iterator().asList())
      {
        addResultEntity(result, true);
      }
      emit resultsModelChanged();
    });
  }
  else if (queryType == "Arriving in PHX")
  {
    m_bufferGraphicsOverlay->setVisible(false);

    // Perform query using where clause on the data source and select returned dynamic entities.
    m_params = new DynamicEntityQueryParameters();
    m_params->setWhereClause("status = 'In flight' AND arrival_airport = 'PHX'");
    m_dataSource->queryDynamicEntitiesAsync(m_params).then(this, [this](DynamicEntityQueryResult* results)
    {
      if (!results)
      {
        return;
      }
      m_resultsModel->clear();
      m_resultEntities.clear();

      for (DynamicEntity* result : results->iterator().asList())
      {
        addResultEntity(result, true);
      }
      emit resultsModelChanged();
    });
  }
  else if (queryType == "Flight Number")
  {
    m_bufferGraphicsOverlay->setVisible(false);
    if (m_resultsModel)
    {
      m_resultsModel->clear();
    }
    m_resultEntities.clear();
    emit resultsModelChanged();
  }
  else
  {
  }
}

void QueryDynamicEntities::runFlightNumberQuery(const QString& flightNumber)
{
  if (!m_dataSource || flightNumber.trimmed().isEmpty())
  {
    return;
  }

  // Perform query using trackId on the data source and select returned dynamic entities.
  m_params = new DynamicEntityQueryParameters();
  m_params->setTrackIds(QStringList{flightNumber.trimmed()});
  m_dataSource->queryDynamicEntitiesAsync(m_params).then(this, [this](DynamicEntityQueryResult* results)
  {
    if (!results)
    {
      return;
    }

    clearEntityUpdateConnections();
    if (m_resultsModel)
    {
      m_resultsModel->clear();
    }
    m_resultEntities.clear();

    Esri::ArcGISRuntime::DynamicEntity* matched = nullptr;
    for (DynamicEntity* result : results->iterator().asList())
    {
      const int before = m_resultEntities.size();
      addResultEntity(result, false);
      if (m_resultEntities.size() > before)
      {
        matched = result;
        break;
      }
    }
    emit resultsModelChanged();

    // Select the matched flight
    if (matched && m_dynamicEntityLayer)
    {
      m_dynamicEntityLayer->clearSelection();
      m_dynamicEntityLayer->selectDynamicEntity(matched);
    }
  });
}

QString QueryDynamicEntities::flightDetailsForRow(int row) const
{
  if (!m_resultsModel)
  {
    return {};
  }
  if (row < 0 || row >= m_resultsModel->rowCount())
  {
    return {};
  }

  const QModelIndex idx = m_resultsModel->index(row, 0);
  const QString status = m_resultsModel->data(idx, FlightInfoListModel::StatusRole).toString();
  const QString arrival = m_resultsModel->data(idx, FlightInfoListModel::ArrivalAirportRole).toString();
  const QString aircraft = m_resultsModel->data(idx, FlightInfoListModel::AircraftRole).toString();
  const QString altitude = m_resultsModel->data(idx, FlightInfoListModel::AltitudeFeetRole).toString();
  const QString speed = m_resultsModel->data(idx, FlightInfoListModel::SpeedRole).toString();
  const QString heading = m_resultsModel->data(idx, FlightInfoListModel::HeadingRole).toString();

  return QString(
           "<b>Status:</b> %1<br><b>Arrival:</b> %2<br><b>Aircraft:</b> %3<br><b>Altitude:</b> %4 ft<br><b>Speed:</b> %5<br><b>Heading:</b> %6°")
    .arg(status, arrival, aircraft, altitude, speed, heading);
}

int QueryDynamicEntities::resultsCount() const
{
  return m_resultsModel ? m_resultsModel->rowCount() : 0;
}
