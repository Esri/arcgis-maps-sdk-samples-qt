// [WriteFile Name=ControlAnnotationSublayerVisibility, Category=DisplayInformation]
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

#ifndef CONTROLANNOTATIONSUBLAYERVISIBILITY_H
#define CONTROLANNOTATIONSUBLAYERVISIBILITY_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class MobileMapPackage;
class AnnotationSublayer;
class LayerListModel;
class Layer;
}
}

#include <QObject>

class ControlAnnotationSublayerVisibility : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QString openLayerText MEMBER m_openLayerText NOTIFY openLayerTextChanged)
  Q_PROPERTY(QString closedLayerText MEMBER m_closedLayerText NOTIFY closedLayerTextChanged)
  Q_PROPERTY(double mapScale MEMBER m_mapScale NOTIFY mapScaleChanged())
  Q_PROPERTY(bool visibleAtCurrentExtent MEMBER m_visibleAtCurrentExtent NOTIFY visibleAtCurrentExtentChanged())

public:
  explicit ControlAnnotationSublayerVisibility(QObject* parent = nullptr);
  ~ControlAnnotationSublayerVisibility();

  static void init();

  Q_INVOKABLE void openLayerVisible();
  Q_INVOKABLE void closedLayerVisible();

signals:
  void mapViewChanged();
  void openLayerTextChanged();
  void closedLayerTextChanged();
  void mapScaleChanged();
  void visibleAtCurrentExtentChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void createMapPackage(const QString& path);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::MobileMapPackage* m_mobileMapPackage = nullptr;
  Esri::ArcGISRuntime::AnnotationSublayer* m_annotationSubLayerOpen = nullptr;
  Esri::ArcGISRuntime::AnnotationSublayer* m_annotationSubLayerClosed = nullptr;
  Esri::ArcGISRuntime::LayerListModel* m_layerListModel = nullptr;
  Esri::ArcGISRuntime::Layer* m_annoLayer = nullptr;

  QString m_openLayerText;
  QString m_closedLayerText;
  bool m_visibleAtCurrentExtent;
  double m_mapScale = 0.0;
};

#endif // CONTROLANNOTATIONSUBLAYERVISIBILITY_H
