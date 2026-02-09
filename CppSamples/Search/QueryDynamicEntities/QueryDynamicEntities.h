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

#ifndef QUERYDYNAMICENTITIES_H
#define QUERYDYNAMICENTITIES_H

// sample headers
#include "FlightInfoListModel.h"

// Qt headers
#include <QObject>

namespace Esri::ArcGISRuntime
{
  class Map;
  class MapQuickView;
  class GraphicsOverlay;
  class Geometry;
  class Polygon;
  class DynamicEntityLayer;
  class Point;
  class CustomDynamicEntityDataSource;
  class DynamicEntityQueryParameters;
  class DynamicEntity;
  class Polygon;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h");

class QueryDynamicEntities : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(FlightInfoListModel* resultsModel READ resultsModel NOTIFY resultsModelChanged)
  Q_PROPERTY(int resultsCount READ resultsCount NOTIFY resultsModelChanged)

public:
  explicit QueryDynamicEntities(QObject* parent = nullptr);
  ~QueryDynamicEntities() override;

  static void init();
  Q_INVOKABLE void handleQuerySelection(const QString& queryType);
  Q_INVOKABLE void runFlightNumberQuery(const QString& flightNumber);
  Q_INVOKABLE int resultsCount() const;
  Q_INVOKABLE QString flightDetailsForRow(int row) const;

signals:
  void mapViewChanged();
  void resultsModelChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void setUpGraphics();
  void setupDynamicEntityDataSource();
  void connectEntityUpdates(Esri::ArcGISRuntime::DynamicEntity* entity, const QString& trackId);
  void clearEntityUpdateConnections();
  void addResultEntity(Esri::ArcGISRuntime::DynamicEntity* result, bool selectEntity);
  FlightInfoListModel* resultsModel() const;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_bufferGraphicsOverlay = nullptr;

  Esri::ArcGISRuntime::Polygon* m_phoenixAirportBuffer = nullptr;
  Esri::ArcGISRuntime::DynamicEntityLayer* m_dynamicEntityLayer = nullptr;
  Esri::ArcGISRuntime::CustomDynamicEntityDataSource* m_dataSource = nullptr;
  Esri::ArcGISRuntime::DynamicEntityQueryParameters* m_params = nullptr;
  FlightInfoListModel* m_resultsModel = nullptr;
  QHash<QString, Esri::ArcGISRuntime::DynamicEntity*> m_resultEntities;
  QHash<QString, QMetaObject::Connection> m_entityConnections;
};

#endif // QUERYDYNAMICENTITIES_H
