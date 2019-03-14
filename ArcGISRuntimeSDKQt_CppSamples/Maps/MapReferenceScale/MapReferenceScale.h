// [WriteFile Name=MapReferenceScale, Category=Maps]
// [Legal]
// Copyright 2018 Esri.

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

#ifndef MAPREFERENCESCALE_H
#define MAPREFERENCESCALE_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class Portal;
class PortalItem;
class LayerListModel;
}
}

#include <QAbstractListModel>
#include <QObject>

class MapReferenceScale : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QAbstractListModel* layerInfoListModel READ layerInfoListModel NOTIFY layerInfoListModelChanged)
  Q_PROPERTY(double mapScale READ mapScale NOTIFY mapScaleChanged)

public:
  explicit MapReferenceScale(QObject* parent = nullptr);
  ~MapReferenceScale();

  static void init();

  Q_INVOKABLE void changeReferenceScale(const double& scale);
  Q_INVOKABLE void setMapScaleToReferenceScale(const double& scale);
  //not working and I don't know why
  Q_INVOKABLE void featureLayerScaleSymbols(const QString& layerName, const bool& checkedStatus);
  Q_INVOKABLE double currentMapScale();


signals:
  void mapViewChanged();
  void layerInfoListModelChanged();
  void mapScaleChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QAbstractListModel* layerInfoListModel() const { return m_layerInfoListModel; }
  double mapScale() { return m_mapScale; }

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Portal* m_portal = nullptr;
  Esri::ArcGISRuntime::PortalItem* m_portalItem = nullptr;
  bool m_portalLoaded = false;
  QAbstractListModel* m_layerInfoListModel = nullptr;
  double m_mapScale = 0.0;
  QString m_mapLoadError;
};

#endif // MAPREFERENCESCALE_H
