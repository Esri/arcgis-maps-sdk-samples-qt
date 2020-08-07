// [WriteFile Name=EditWithBranchVersioning, Category=EditData]
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

#ifndef EDITWITHBRANCHVERSIONING_H
#define EDITWITHBRANCHVERSIONING_H

namespace Esri
{
namespace ArcGISRuntime
{
class AuthenticationManager;
class ServiceVersionParameters;
class Map;
class MapQuickView;
class ServiceGeodatabase;
class FeatureLayer;
class ServiceFeatureTable;
class ArcGISFeature;
}
}

#include <QObject>
#include <QUrl>

class EditWithBranchVersioning : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::AuthenticationManager* authManager READ authManager NOTIFY authManagerChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QString sgdbCurrentVersion MEMBER m_sgdbCurrentVersion NOTIFY sgdbCurrentVersionChanged)

  Q_PROPERTY(QString featureType READ featureType NOTIFY featureTypeChanged)

public:
  explicit EditWithBranchVersioning(QObject* parent = nullptr);
  ~EditWithBranchVersioning();

  static void init();

  Esri::ArcGISRuntime::AuthenticationManager* authManager() const;
  Esri::ArcGISRuntime::ServiceVersionParameters* createParams();

  Q_INVOKABLE void createVersion();
  Q_INVOKABLE void switchVersion() const;

  // to be removed
  Q_INVOKABLE void fetchVersions() const;

signals:
  void authManagerChanged();
  void mapViewChanged();
  void sgdbCurrentVersionChanged();
  void featureSelected();
  void featureTypeChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QString featureType() const;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ServiceGeodatabase* m_serviceGeodatabase = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
  Esri::ArcGISRuntime::ArcGISFeature* m_selectedFeature = nullptr;

  const QUrl m_serviceUrl{"https://rt-server1081.esri.com/portal/home/item.html?id=adb5c3090edf43f3853e57d8b0810f9b"};

  QString m_sgdbCurrentVersion;
  QString m_createdVersion;
  QString m_featureType;
};

#endif // EDITWITHBRANCHVERSIONING_H
