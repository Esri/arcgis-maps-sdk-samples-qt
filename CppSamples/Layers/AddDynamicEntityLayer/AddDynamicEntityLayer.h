// [WriteFile Name=AddDynamicEntityLayer, Category=Layers]
// [Legal]
// Copyright 2023 Esri.
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

#ifndef ADDDYNAMICENTITYLAYER_H
#define ADDDYNAMICENTITYLAYER_H

// Qt headers
#include <QObject>

namespace Esri::ArcGISRuntime
{
class ArcGISStreamService;
class DynamicEntityLayer;
class Map;
class MapQuickView;
}

class QMouseEvent;

Q_MOC_INCLUDE("MapQuickView.h");

class AddDynamicEntityLayer : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)

public:
  explicit AddDynamicEntityLayer(QObject* parent = nullptr);
  ~AddDynamicEntityLayer() override;

  static void init();

  Q_INVOKABLE void purgeAllObservations();
  Q_INVOKABLE void setObservationsPerTrack(int observationsPerTrack);
  Q_INVOKABLE void showTrackLines(bool showTrackLines);
  Q_INVOKABLE void showPreviousObservations(bool showPreviousObservations);
  Q_INVOKABLE void enableDisableConnection();

signals:
  void mapViewChanged();
  void connectionStatusChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  QString connectionStatus() const;

  void identifyLayerAtMouseClick(const QMouseEvent& e);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::ArcGISStreamService* m_dynamicEntityDataSource = nullptr;
  Esri::ArcGISRuntime::DynamicEntityLayer* m_dynamicEntityLayer = nullptr;
};

#endif // ADDDYNAMICENTITYLAYER_H
