// [WriteFile Name=ClassBreaksWithAlternateSymbols, Category=Layers]
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

#ifndef CLASSBREAKSWITHALTERNATESYMBOLS_H
#define CLASSBREAKSWITHALTERNATESYMBOLS_H

namespace Esri
{
namespace ArcGISRuntime
{
class FeatureLayer;
class Map;
class MapQuickView;
class ServiceFeatureTable;
class Renderer;
class ClassBreaksRenderer;
class ArcGISMapImageSublayer;
class ClassBreak;
class MultilayerPolygonSymbol;
class Symbol;
class SimpleFillSymbol;
}
}

#include <QObject>

class ClassBreaksWithAlternateSymbols : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit ClassBreaksWithAlternateSymbols(QObject* parent = nullptr);
  ~ClassBreaksWithAlternateSymbols();

  static void init();
  Q_INVOKABLE void setScale(double scale);

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void createClassBreaksRenderer();
    QList<Esri::ArcGISRuntime::Symbol*> createAlternateSymbols();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::ClassBreaksRenderer* m_classBreaksRenderer = nullptr;
//  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
};

#endif // CLASSBREAKSWITHALTERNATESYMBOLS_H