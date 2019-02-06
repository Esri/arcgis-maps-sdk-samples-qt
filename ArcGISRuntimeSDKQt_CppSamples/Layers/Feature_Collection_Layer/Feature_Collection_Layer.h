// [WriteFile Name=Feature_Collection_Layer, Category=Layers]
// [Legal]
// Copyright 2016 Esri.

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

#ifndef FEATURE_COLLECTION_LAYER_H
#define FEATURE_COLLECTION_LAYER_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class FeatureCollection;
    class FeatureCollectionLayer;
    class FeatureCollectionTable;
  }
}

#include "Point.h"
#include <QQuickItem>
#include <QList>

class Feature_Collection_Layer : public QQuickItem
{
  Q_OBJECT

public:
  explicit Feature_Collection_Layer(QQuickItem* parent = nullptr);
  ~Feature_Collection_Layer() override;

  void componentComplete() override;
  static void init();

private:
  void createPointTable();
  void createPolylineTable();
  void createPolygonTable();
  void addPointToTable(QString attrName, QString attrValue, Esri::ArcGISRuntime::Point point, Esri::ArcGISRuntime::FeatureCollectionTable* table);
  void addPolylineToTable(QString attrName, QString attrValue, QList<Esri::ArcGISRuntime::Point> points, Esri::ArcGISRuntime::FeatureCollectionTable* table);
  void addPolygonToTable(QString attrName, QString attrValue, QList<Esri::ArcGISRuntime::Point> points, Esri::ArcGISRuntime::FeatureCollectionTable* table);

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureCollection* m_featureCollection = nullptr;
  Esri::ArcGISRuntime::FeatureCollectionLayer* m_featureCollectionLayer = nullptr;
};

#endif // FEATURE_COLLECTION_LAYER_H
