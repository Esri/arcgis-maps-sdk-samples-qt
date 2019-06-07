// [WriteFile Name=BrowseWfsLayers, Category=Layers]
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

#ifndef BROWSEWFSLAYERS_H
#define BROWSEWFSLAYERS_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class WfsService;
class WfsFeatureTable;
class WfsLayerInfo;
class LayerListModel;
}
}

#include <QObject>
#include <QList>
#include <QStringListModel>

class BrowseWfsLayers : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QStringList layerInfoTitleListModel MEMBER m_layerInfoTitleList NOTIFY layerInfoTitleListChanged)
  Q_PROPERTY(bool isLoading MEMBER m_loadingIndicator NOTIFY isLoadingChanged)

public:
  explicit BrowseWfsLayers(QObject* parent = nullptr);
  ~BrowseWfsLayers();

  static void init();

  Q_INVOKABLE void createWfsFeatureTable(int index, bool swap);

signals:
  void mapViewChanged();
  void layerInfoTitleListChanged();
  void isLoadingChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void populateWfsFeatureTable();
  void addFeatureLayerToMap();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::WfsService* m_wfsService = nullptr;
  Esri::ArcGISRuntime::WfsFeatureTable* m_wfsFeatureTable = nullptr;
  QList<Esri::ArcGISRuntime::WfsLayerInfo> m_wfsLayersInfoList;
  QStringList m_layerInfoTitleList;
  bool m_loadingIndicator = false;
  bool m_swapAxis = false;


};

#endif // BROWSEWFSLAYERS_H
