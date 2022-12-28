// [WriteFile Name=DisplayFeatureLayers, Category=Layers]
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

#ifndef DISPLAYFEATURELAYERS_H
#define DISPLAYFEATURELAYERS_H

namespace Esri::ArcGISRuntime
{
class FeatureLayer;
class Map;
class MapQuickView;
}

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h");

class DisplayFeatureLayers : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit DisplayFeatureLayers(QObject* parent = nullptr);
  ~DisplayFeatureLayers() override;

  enum class FeatureLayerMode {
    Geodatabase,
    Geopackage,
    PortalItem,
    ServiceFeatureTable,
    Shapefile
  };

  Q_ENUM(FeatureLayerMode)

  static void init();

  Q_INVOKABLE void setLayerMode(DisplayFeatureLayers::FeatureLayerMode featureLayerMode);

signals:
  void mapViewChanged();

private:
  [[nodiscard]] Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  void setGeodatabaseLayer();
  void setGeopackageLayer();
  void setPortalItemLayer();
  void setServiceFeatureTableLayer();
  void setShapefileLayer();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  DisplayFeatureLayers::FeatureLayerMode m_currentLayerMode;
};

#endif // DISPLAYFEATURELAYERS_H
