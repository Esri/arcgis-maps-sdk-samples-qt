// [WriteFile Name=Geotriggers, Category=Analysis]
// [Legal]
// Copyright 2021 Esri.

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

namespace Esri::ArcGISRuntime
{
class ArcGISFeature;
class GeotriggerFeed;
class GeotriggerMonitor;
class GeotriggerNotificationInfo;
class Graphic;
class GraphicsOverlay;
class Map;
class MapQuickView;
class Point;
class ServiceFeatureTable;
class SimulatedLocationDataSource;
}

#include <QObject>
#include <QMap>
#include <QUrl>

Q_MOC_INCLUDE("MapQuickView.h")

class Geotriggers : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QString currentSection READ currentSection NOTIFY displayInfoChanged)
  Q_PROPERTY(QStringList poisInRange READ poisInRange NOTIFY displayInfoChanged)
  Q_PROPERTY(QString currentDescription READ currentDescription NOTIFY displayInfoChanged)
  Q_PROPERTY(QUrl currentImageUrl READ currentImageUrl NOTIFY displayInfoChanged)

public:
  explicit Geotriggers(QObject* parent = nullptr);
  ~Geotriggers();

  static void init();

  Q_INVOKABLE void getFeatureInformation(const QString &featureName);

signals:
  void mapViewChanged();
  void displayInfoChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  QString currentSection() const;
  QStringList poisInRange() const;
  QString currentDescription() const;
  QUrl currentImageUrl() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  void createGeotriggerMonitor(Esri::ArcGISRuntime::ServiceFeatureTable* serviceFeatureTable, double bufferSize, const QString &geotriggerName);
  void initializeSimulatedLocationDisplay();
  void handleGeotriggerNotification(Esri::ArcGISRuntime::GeotriggerNotificationInfo* geotriggerNotificationInfo);

  Esri::ArcGISRuntime::GeotriggerFeed* m_geotriggerFeed = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_gardenSections = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_gardenPois = nullptr;
  Esri::ArcGISRuntime::SimulatedLocationDataSource* m_simulatedLocationDataSource = nullptr;

  QMap<QString, QStringList> m_currentFeaturesEntered;
  QString m_currentFeatureDescription;
  QUrl m_currentFeatureImageUrl;
  QMap<QString, Esri::ArcGISRuntime::ArcGISFeature*> m_featureQMap;
  QMap<QString, QUrl> m_featureAttachmentImageUrls;
};

#endif // GEOTRIGGERS_H
