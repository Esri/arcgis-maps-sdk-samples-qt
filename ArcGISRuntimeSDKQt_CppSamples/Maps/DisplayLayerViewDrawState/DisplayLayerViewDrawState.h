// [WriteFile Name=DisplayLayerViewDrawState, Category=Maps]
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

#ifndef DISPLAYLAYERVIEWDRAWSTATE_H
#define DISPLAYLAYERVIEWDRAWSTATE_H

namespace Esri
{
namespace ArcGISRuntime
{
class FeatureLayer;
class Map;
class MapQuickView;
class PortalItem;
class Layer;
class LayerViewState;
}
}

#include <QObject>

class DisplayLayerViewDrawState : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QStringList viewStatus MEMBER m_viewStatuses NOTIFY viewStatusChanged)
  Q_PROPERTY(QString warningMessage MEMBER m_warningMessage NOTIFY warningMessageChanged)
  Q_PROPERTY(bool loading MEMBER m_loading NOTIFY loadingChanged)

public:
  explicit DisplayLayerViewDrawState(QObject* parent = nullptr);
  ~DisplayLayerViewDrawState();

  static void init();
  void onLayerViewStateCompleted(Esri::ArcGISRuntime::Layer* layer, Esri::ArcGISRuntime::LayerViewState layerViewState);

  Q_INVOKABLE void loadLayer();
  Q_INVOKABLE void changeFeatureLayerVisibility(bool visible);

signals:
  void mapViewChanged();
  void viewStatusChanged();
  void loadingChanged();
  void warningMessageChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::PortalItem* m_portalItem = nullptr;

  bool m_loading = false;
  QStringList m_viewStatuses;
  QString m_warningMessage;
};

#endif // DISPLAYLAYERVIEWDRAWSTATE_H
