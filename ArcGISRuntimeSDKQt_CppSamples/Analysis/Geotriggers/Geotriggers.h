// [WriteFile Name=Geotriggers, Category=Analysis]
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

#ifndef GEOTRIGGERS_H
#define GEOTRIGGERS_H

#include "Point.h"

namespace Esri
{
namespace ArcGISRuntime
{
class ArcGISFeature;
class AttachmentListModel;
class GeotriggerMonitor;
class Graphic;
class GraphicsOverlay;
class Map;
class MapQuickView;
class Point;
class ServiceFeatureTable;
class SimulatedLocationDataSource;
}
}

#include <QObject>

class Geotriggers : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QStringList nearbySections READ nearbySections NOTIFY sectionInfoChanged)
  Q_PROPERTY(QString sectionDescription READ sectionDescription WRITE setSectionDescription NOTIFY sectionInfoChanged)
  Q_PROPERTY(QUrl sectionImageUrl READ sectionImageUrl WRITE setSectionImageUrl NOTIFY sectionInfoChanged)

public:
  explicit Geotriggers(QObject* parent = nullptr);
  ~Geotriggers();

  static void init();

  Q_INVOKABLE void getSectionInformation(QString sectionName);

signals:
  void mapViewChanged();
  void sectionInfoChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  QStringList nearbySections() const;
  QString sectionDescription() const;
  QUrl sectionImageUrl() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void setSectionDescription(QString sectionDescription);
  void setSectionImageUrl(QUrl sectionImageUrl);
  void setupGeotriggerMonitor();
  void initializeSimulatedLocationDisplay();
  void createAttachmentConnection();

  Esri::ArcGISRuntime::ArcGISFeature* m_agsFeature = nullptr;
  Esri::ArcGISRuntime::AttachmentListModel* m_featureAttachments = nullptr;
  Esri::ArcGISRuntime::GeotriggerMonitor* m_geotriggerMonitor = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Graphic* m_locationHistoryLineGraphic = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_gardenSections = nullptr;
  Esri::ArcGISRuntime::SimulatedLocationDataSource* m_simulatedLocationDataSource = nullptr;
  QStringList m_nearbySections;
  QString m_sectionName;
  QString m_sectionDescription;
  QUrl m_sectionImageUrl;
};

#endif // GEOTRIGGERS_H
