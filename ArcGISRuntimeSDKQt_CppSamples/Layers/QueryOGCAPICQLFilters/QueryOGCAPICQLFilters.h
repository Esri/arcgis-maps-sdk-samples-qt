// [WriteFile Name=QueryOGCAPICQLFiltersQuery_OGC, Category=Layers]
// [Legal]
// Copyright 2021 Esri.

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

#ifndef QUERYOGCAPICQLFILTERS_H
#define QUERYOGCAPICQLFILTERS_H

#include "Envelope.h"
#include "TaskWatcher.h"

namespace Esri
{
namespace ArcGISRuntime
{
class FeatureLayer;
class Map;
class MapQuickView;
class OgcFeatureCollectionTable;
}
}

#include <QObject>

class QueryOGCAPICQLFilters : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit QueryOGCAPICQLFilters(QObject* parent = nullptr);
  ~QueryOGCAPICQLFilters();

  static void init();
  Q_INVOKABLE void query(const QString& whereClause, const QString& maxFeature, const QString& fromDateString, const QString& toDateString);

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Envelope m_dataSetExtent;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::OgcFeatureCollectionTable* m_ogcFeatureCollectionTable = nullptr;
};

#endif // QUERYOGCAPICQLFILTERS_H
