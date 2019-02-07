// [WriteFile Name=QueryMapImageSublayer, Category=Layers]
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

#ifndef QUERYMAPIMAGESUBLAYER_H
#define QUERYMAPIMAGESUBLAYER_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class ServiceFeatureTable;
class ArcGISMapImageLayer;
class GraphicsOverlay;
class SimpleMarkerSymbol;
class SimpleFillSymbol;
class Symbol;
class FeatureQueryResult;
}
}

#include <QQuickItem>

class QueryMapImageSublayer : public QQuickItem
{
  Q_OBJECT

public:
  explicit QueryMapImageSublayer(QQuickItem* parent = nullptr);
  ~QueryMapImageSublayer() override = default;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void query(const QString& whereClause);

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_citiesTable = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_countiesTable = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_statesTable = nullptr;
  Esri::ArcGISRuntime::ArcGISMapImageLayer* m_usaImageLayer = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_selectionOverlay = nullptr;
  Esri::ArcGISRuntime::SimpleFillSymbol* m_stateSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleFillSymbol* m_countySymbol = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_citySymbol = nullptr;
  void connectSignals();
  void createSymbols();
  void addResultsAsGraphics(Esri::ArcGISRuntime::FeatureQueryResult* result, Esri::ArcGISRuntime::Symbol* symbol);
};

#endif // QUERYMAPIMAGESUBLAYER_H
