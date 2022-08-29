// [WriteFile Name=DisplayRouteLayer, Category=Routing]
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

#ifndef DISPLAYROUTELAYER_H
#define DISPLAYROUTELAYER_H

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

#include <QObject>

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

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QString directions() const;
//  QString getDirections();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::PortalItem* m_portalItem = nullptr;
  Esri::ArcGISRuntime::FeatureCollection* m_featureCollection;
  Esri::ArcGISRuntime::FeatureCollectionLayer* m_featureCollectionLayer;
  Esri::ArcGISRuntime::FeatureTable* m_directionsTable;
  Esri::ArcGISRuntime::FeatureQueryResult* m_featureQueryResult;
  Esri::ArcGISRuntime::Feature* m_feature;
  QString m_featureDirection = "";
//  Esri::ArcGISRuntime::FeatureCollectionTableListModel m_featureCollectionTable;
};

#endif // DISPLAYROUTELAYER_H
