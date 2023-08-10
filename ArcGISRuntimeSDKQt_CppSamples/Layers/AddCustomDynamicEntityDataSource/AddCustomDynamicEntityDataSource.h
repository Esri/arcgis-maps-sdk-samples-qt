// [WriteFile Name=AddCustomDynamicEntityDataSource, Category=Layers]
// [Legal]
// Copyright 2023 Esri.

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

#ifndef ADDCUSTOMDYNAMICENTITYDATASOURCE_H
#define ADDCUSTOMDYNAMICENTITYDATASOURCE_H

namespace Esri::ArcGISRuntime
{
class DynamicEntity;
class DynamicEntityLayer;
class Map;
class MapQuickView;
}

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h");

class AddCustomDynamicEntityDataSource : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit AddCustomDynamicEntityDataSource(QObject* parent = nullptr);
  ~AddCustomDynamicEntityDataSource() override;

  static void init();

  Q_INVOKABLE void purgeAllObservations();

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::DynamicEntity* m_dynamicEntity = nullptr;
  Esri::ArcGISRuntime::DynamicEntityLayer* m_dynamicEntityLayer = nullptr;

};

#endif // ADDCUSTOMDYNAMICENTITYDATASOURCE_H
