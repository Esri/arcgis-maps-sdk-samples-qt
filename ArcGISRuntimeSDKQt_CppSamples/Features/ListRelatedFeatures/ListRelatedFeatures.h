// [WriteFile Name=ListRelatedFeatures, Category=Features]
// [Legal]
// Copyright 2017 Esri.

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

#ifndef LISTRELATEDFEATURES_H
#define LISTRELATEDFEATURES_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class ArcGISFeatureTable;
    class FeatureLayer;
    class Map;
    class MapQuickView;
  }
}
class RelatedFeatureListModel;

#include <QQuickItem>

class ListRelatedFeatures : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(RelatedFeatureListModel* relatedFeaturesModel READ relatedFeaturesModel NOTIFY relatedFeaturesModelChanged)

public:
  explicit ListRelatedFeatures(QQuickItem* parent = nullptr);
  ~ListRelatedFeatures() override = default;

  void componentComplete() override;
  static void init();

signals:
  void showAttributeTable();
  void hideAttributeTable();
  void relatedFeaturesModelChanged();

private:
  void connectSignals();
  RelatedFeatureListModel* relatedFeaturesModel() const { return m_relatedFeaturesModel; }

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_alaskaNationalParks = nullptr;  
  Esri::ArcGISRuntime::ArcGISFeatureTable* m_alaskaFeatureTable = nullptr;
  RelatedFeatureListModel* m_relatedFeaturesModel = nullptr;
};

#endif // LISTRELATEDFEATURES_H
