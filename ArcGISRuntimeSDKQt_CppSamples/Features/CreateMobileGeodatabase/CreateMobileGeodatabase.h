// [WriteFile Name=CreateMobileGeodatabase, Category=Features]
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

#ifndef CREATEMOBILEGEODATABASE_H
#define CREATEMOBILEGEODATABASE_H

namespace Esri
{
namespace ArcGISRuntime
{
class FeatureTable;
class Geodatabase;
class Map;
class MapQuickView;
}
}

#include <FeatureListModel.h>

#include <QObject>
#include <QMouseEvent>
#include <QTemporaryDir>

class CreateMobileGeodatabase : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(FeatureListModel* featureListModel READ featureListModel NOTIFY featureListModelChanged)

public:
  explicit CreateMobileGeodatabase(QObject* parent = nullptr);
  ~CreateMobileGeodatabase();

  static void init();

  Q_INVOKABLE void test();
  Q_INVOKABLE void clearTable();

signals:
  void mapViewChanged();
  void featureListModelChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  FeatureListModel* featureListModel() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void createGeodatabase();
  void createTable();
  void addFeature(QMouseEvent mouseEvent);
  void deleteFeatures();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureTable* m_featureTable = nullptr;
  Esri::ArcGISRuntime::Geodatabase* m_gdb = nullptr;
  FeatureListModel* m_featureListModel = nullptr;

  QTemporaryDir m_tempDir;
};

#endif // CREATEMOBILEGEODATABASE_H
