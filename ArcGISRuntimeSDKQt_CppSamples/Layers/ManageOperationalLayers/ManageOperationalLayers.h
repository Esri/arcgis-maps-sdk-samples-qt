// [WriteFile Name=ManageOperationalLayers, Category=Layers]
// [Legal]
// Copyright 2019 Esri.

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

#ifndef MANAGEOPERATIONALLAYERS_H
#define MANAGEOPERATIONALLAYERS_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class ArcGISMapImageLayer;
class Layer;
}
}

class DrawOrderLayerListModel;
class QAbstractItemModel;

#include <QObject>

class ManageOperationalLayers : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QAbstractItemModel* layerListModel READ layerListModel NOTIFY layerListModelChanged)
  Q_PROPERTY(QStringList deletedLayersList MEMBER m_deletedLayersListNames NOTIFY deletedLayersListChanged)

public:
  explicit ManageOperationalLayers(QObject* parent = nullptr);
  ~ManageOperationalLayers();

  static void init();
  Q_INVOKABLE void moveLayerUp(int index);
  Q_INVOKABLE void moveLayerDown(int index);
  Q_INVOKABLE void removeLayer(int index);
  Q_INVOKABLE void addLayer(int index);

signals:
  void mapViewChanged();
  void layerListModelChanged();
  void deletedLayersListChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  QAbstractItemModel* layerListModel() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ArcGISMapImageLayer* m_elevationLayer = nullptr;
  Esri::ArcGISRuntime::ArcGISMapImageLayer* m_censusLayer = nullptr;
  Esri::ArcGISRuntime::ArcGISMapImageLayer* m_damageLayer = nullptr;
  DrawOrderLayerListModel* m_drawOrderListModel = nullptr;
  QStringList m_deletedLayersListNames;
  QList<Esri::ArcGISRuntime::Layer*> m_deletedLayers;
};

#endif // MANAGEOPERATIONALLAYERS_H
