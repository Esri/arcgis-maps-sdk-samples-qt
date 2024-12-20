// [WriteFile Name=DisplayRouteLayer, Category=Routing]
// [Legal]
// Copyright 2022 Esri.
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

#ifndef DISPLAYROUTELAYER_H
#define DISPLAYROUTELAYER_H

// Qt headers
#include <QObject>

// STL headers
#include <FeatureCollectionTableListModel.h>

namespace Esri
{
namespace ArcGISRuntime
{
class Feature;
class FeatureCollection;
class FeatureCollectionLayer;
class FeatureCollectionTableListModel;
class FeatureTable;
class Map;
class MapQuickView;
class PortalItem;
}
}

Q_MOC_INCLUDE("MapQuickView.h")

class DisplayRouteLayer : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QString directions READ directions NOTIFY directionsChanged)

public:
  explicit DisplayRouteLayer(QObject* parent = nullptr);
  ~DisplayRouteLayer();

  static void init();
  Q_INVOKABLE void getDirections();

signals:
  void mapViewChanged();
  void directionsChanged();
  void enableDirectionsButton();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QString directions() const;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::PortalItem* m_portalItem = nullptr;
  Esri::ArcGISRuntime::FeatureCollection* m_featureCollection = nullptr;
  Esri::ArcGISRuntime::FeatureCollectionLayer* m_featureCollectionLayer = nullptr;
  Esri::ArcGISRuntime::Feature* m_feature = nullptr;
  QString m_featureDirection = "";
};

#endif // DISPLAYROUTELAYER_H
